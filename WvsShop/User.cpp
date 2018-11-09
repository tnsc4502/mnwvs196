#include "User.h"
#include "ClientSocket.h"
#include "WvsShop.h"

#include "..\Database\GA_Character.hpp"
#include "..\Database\GW_CharacterStat.h"
#include "..\Database\GW_CharacterLevel.h"
#include "..\Database\GW_CharacterMoney.h"
#include "..\Database\GW_ItemSlotEquip.h"
#include "..\Database\GW_ItemSlotPet.h"
#include "..\Database\GW_ItemSlotBundle.h"
#include "..\Database\GW_Avatar.hpp"
#include "..\Database\GW_CashItemInfo.h"
#include "..\Database\GW_FuncKeyMapped.h"

#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Net\InPacket.h"

#include "..\WvsLib\Net\PacketFlags\UserPacketFlags.hpp"
#include "..\WvsLib\Net\PacketFlags\ShopPacketFlags.hpp"
#include "..\WvsLib\Task\AsyncScheduler.h"

#include "..\WvsGame\ItemInfo.h"
#include "ShopInfo.h"

User::User(ClientSocket *_pSocket, InPacket *iPacket)
	: m_pSocket(_pSocket),
	m_pCharacterData(AllocObj(GA_Character))
{
	_pSocket->SetUser(this);
	m_pFuncKeyMapped = AllocObjCtor(GW_FuncKeyMapped)(m_pCharacterData->nCharacterID);
	m_pCharacterData->nAccountID = iPacket->Decode4();
	m_pCharacterData->DecodeCharacterData(iPacket, true);
	m_pFuncKeyMapped->Decode(iPacket);
	if (!iPacket->Decode1())
		m_nChannelID = 0;
	else
		m_nChannelID = iPacket->Decode4();
	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer = AsyncScheduler::CreateTask(bindT, 2000, true);
	m_pUpdateTimer->Start();

	OnQueryCashRequest();
	OnRequestCenterUpdateCash();
	OnRequestCenterLoadLocker();
	//ValidateState();
	//m_pSecondaryStat->DecodeInternal(this, iPacket);
}

User::~User()
{
	WvsLogger::LogRaw("Migrate Out, Encoding Character data to center.\n");
	OutPacket oPacket;
	oPacket.Encode2((short)ShopInternalPacketFlag::RequestMigrateOut);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(GetUserID());
	m_pCharacterData->EncodeCharacterData(&oPacket, true);
	m_pFuncKeyMapped->Encode(&oPacket, true);

	oPacket.Encode1(2); //bGameEnd, Dont decode and save the secondarystat info.
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);

	auto bindT = std::bind(&User::Update, this);
	m_pUpdateTimer->Abort();

	FreeObj(m_pUpdateTimer);
	FreeObj(m_pCharacterData);
	FreeObj(m_pFuncKeyMapped);
}

User * User::FindUser(int nUserID)
{
	return WvsBase::GetInstance<WvsShop>()->FindUser(nUserID);
}

int User::GetUserID() const
{
	return m_pCharacterData->nCharacterID;
}

void User::SendPacket(OutPacket *oPacket)
{
	m_pSocket->SendPacket(oPacket);
}

void User::Update()
{
}

void User::OnMigrateOutCashShop()
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopInternalPacketFlag::RequestTransferToGame);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(m_pCharacterData->nCharacterID);
	oPacket.Encode1(m_nChannelID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::ValidateState()
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketFlag::Client_ValidateState);
	oPacket.Encode1(1);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void User::OnQueryCashRequest()
{
	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketFlag::User_QueryCashResult);
	oPacket.Encode4(nNexonCash);
	oPacket.Encode4(nMaplePoint);
	oPacket.Encode4(0);
	oPacket.Encode4(0);
	SendPacket(&oPacket);
}

void User::OnPacket(InPacket *iPacket)
{
	int nType = (unsigned short)iPacket->Decode2();
	switch (nType)
	{
		case UserRecvPacketFlag::User_OnUserTransferFieldRequest:
			OnMigrateOutCashShop();
			break;
		case ShopRecvPacketFlag::User_OnCashItemRequest:
			OnUserCashItemRequest(iPacket);
			break;
		case ShopRecvPacketFlag::User_OnQueryCashReques:
			OnQueryCashRequest();
			break;
	}
}

void User::OnUserCashItemRequest(InPacket * iPacket)
{
	int nType = iPacket->Decode1();
	switch (nType)
	{
		case CashItemRequest::Recv_OnCashItemReqBuy:
			OnRequestBuyCashItem(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqMoveItemToSlot:
			OnRequestMoveItemToSlot(iPacket);
			break;
		case CashItemRequest::Recv_OnCashItemReqMoveItemToLocker:
			OnRequestMoveItemToLocker(iPacket);
			break;
	}
}

void User::OnCenterCashItemResult(int nType, InPacket * iPacket)
{
	bool bValidate = true;
	switch (nType)
	{
		case ShopInternalPacketFlag::OnCenterLoadLockerDone:
			OnCenterResLoadLockerDone(iPacket);
			break;
		case ShopInternalPacketFlag::OnCenterResBuyDone:
			OnCenterResBuyDone(iPacket);
			break;
		case ShopInternalPacketFlag::OnCenterUpdateCashDone:
			OnCenterUpdateCashDone(iPacket);
			break;
		case ShopInternalPacketFlag::OnCenterMoveToSlotDone:
			OnCenterMoveItemToSlotDone(iPacket);
			break;
		case ShopInternalPacketFlag::OnCenterMoveToLockerDone:
			OnCenterMoveItemToLockerDone(iPacket);
			break;
		default:
			bValidate = false;
			break;
	}
	OnQueryCashRequest();
	if(bValidate)
		ValidateState();
}

void User::OnCenterResLoadLockerDone(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketFlag::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResLoadLockerDone);
	oPacket.Encode1(0);
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount());
	SendPacket(&oPacket);
}

void User::OnCenterResBuyDone(InPacket * iPacket)
{
	nNexonCash = iPacket->Decode4();
	nMaplePoint = iPacket->Decode4();

	OutPacket oPacket;
	oPacket.Encode2(ShopSendPacketFlag::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResBuyDone);

	//Transfer GW_CashItemInfo
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);

	oPacket.EncodeBuffer(nullptr, 25);
	SendPacket(&oPacket);
}

void User::OnCenterUpdateCashDone(InPacket * iPacket)
{
	nNexonCash = iPacket->Decode4();
	nMaplePoint = iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();
	OnQueryCashRequest();
}

void User::OnCenterMoveItemToSlotDone(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketFlag::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResMoveLtoSDone);
	long long int liItemSN = iPacket->Decode8();
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);
	iPacket->Decode1();
	int nPOS = iPacket->Decode2();
	int nInstanceType = iPacket->Decode1();
	iPacket->Offset(-1);
	GW_ItemSlotBase *pItem = GW_ItemSlotBase::CreateItem(nInstanceType);
	if (pItem)
	{
		pItem->Decode(iPacket, false);
		pItem->liItemSN = liItemSN;
		pItem->nCharacterID = m_pCharacterData->nCharacterID;
		pItem->nPOS = nPOS;
		pItem->nType = (GW_ItemSlotBase::GW_ItemSlotType)((pItem->nItemID / 1000000));
		m_pCharacterData->mItemSlot[pItem->nType][nPOS] = pItem;
	}

	SendPacket(&oPacket);
}

void User::OnCenterMoveItemToLockerDone(InPacket * iPacket)
{
	auto liCashItemSN = iPacket->Decode8();
	int nType = iPacket->Decode1();
	short nPOS = m_pCharacterData->FindCashItemSlotPosition(nType, liCashItemSN);
	if (nPOS)
	{
		auto pItem = m_pCharacterData->mItemSlot[nType][nPOS];
		m_pCharacterData->mItemSlot[nType].erase(nPOS);
		FreeObj(pItem);
	}

	OutPacket oPacket;
	oPacket.Encode2((short)ShopSendPacketFlag::User_CashItemResult);
	oPacket.Encode1(CashItemRequest::Send_OnCashItemResMoveStoLDone);
	oPacket.EncodeBuffer(
		iPacket->GetPacket() + iPacket->GetReadCount(),
		iPacket->GetPacketSize() - iPacket->GetReadCount()
	);
	SendPacket(&oPacket);
}

void User::OnRequestCenterLoadLocker()
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopInternalPacketFlag::RequestLoadLocker);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnRequestCenterUpdateCash()
{
	OutPacket oPacket;
	oPacket.Encode2((short)ShopInternalPacketFlag::RequestUpdateCash);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnRequestBuyCashItem(InPacket * iPacket)
{
	int nChargeType = iPacket->Decode1();
	iPacket->Decode2();
	auto pCommodity = ShopInfo::GetInstance()->GetCSCommodity(iPacket->Decode4());
	if (pCommodity)
	{
		auto pItem = ItemInfo::GetInstance()->GetItemSlot(
			pCommodity->nItemID,
			ItemInfo::ItemVariationOption::ITEMVARIATION_NONE);

		if (pItem)
		{
			/*
			Sending packets to Center, request it to charge cash points and save the item.
			*/
			OutPacket oPacket;
			oPacket.Encode2((short)ShopInternalPacketFlag::RequestBuyCashItem);
			oPacket.Encode4(m_pSocket->GetSocketID());
			oPacket.Encode4(this->m_pCharacterData->nCharacterID);
			oPacket.Encode4(this->m_pCharacterData->nAccountID);
			oPacket.Encode1(nChargeType);
			oPacket.Encode1(pCommodity->nItemID / 1000000);
			oPacket.Encode1(pItem->bIsPet);
			oPacket.Encode4(pCommodity->nPrice);
			pItem->Encode(&oPacket, false);
			auto pCashItemInfo = ShopInfo::GetInstance()->GetCashItemInfo(pCommodity);
			pCashItemInfo->Encode(&oPacket);
			FreeObj(pCashItemInfo);
			WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
		}
		pItem->Release();
	}
}

void User::OnRequestMoveItemToSlot(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(ShopInternalPacketFlag::RequestMoveLToS);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	oPacket.Encode8(iPacket->Decode8()); //liCashItemSN
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}

void User::OnRequestMoveItemToLocker(InPacket * iPacket)
{
	OutPacket oPacket;
	oPacket.Encode2(ShopInternalPacketFlag::RequestMoveSToL);
	oPacket.Encode4(m_pSocket->GetSocketID());
	oPacket.Encode4(this->m_pCharacterData->nCharacterID);
	oPacket.Encode4(this->m_pCharacterData->nAccountID);
	oPacket.Encode8(iPacket->Decode8()); //liCashItemSN
	oPacket.Encode1(iPacket->Decode1()); //nType
	WvsBase::GetInstance<WvsShop>()->GetCenter()->SendPacket(&oPacket);
}
