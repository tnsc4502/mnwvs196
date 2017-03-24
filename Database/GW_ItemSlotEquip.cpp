#include "GW_ItemSlotEquip.h"
#include "WvsUnified.h"


GW_ItemSlotEquip::GW_ItemSlotEquip()
{
}


GW_ItemSlotEquip::~GW_ItemSlotEquip()
{
}

void GW_ItemSlotEquip::Load(int SN)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "SELECT * FROM ItemSlot_EQP Where SN = " << SN;
	queryStatement.execute();

	Poco::Data::RecordSet recordSet(queryStatement);
	lItemSN = recordSet["ItemSN"];
	nItemID = recordSet["ItemID"];
	lExpireDate = recordSet["ExpireDate"];
	nAttribute = recordSet["Attribute"];
	nPOS = recordSet["POS"];
	nRUC = recordSet["RUC"];
	nCUC = recordSet["CUC"];
	nI_STR = recordSet["I_STR"];
	nI_DEX = recordSet["I_DEX"];
	nI_INT = recordSet["I_INT"];
	nI_LUK = recordSet["I_LUK"];
	nI_MaxHP = recordSet["I_MaxHP"];
	nI_MaxMP = recordSet["I_MaxMP"];
	nI_PAD = recordSet["I_PAD"];
	nI_MAD = recordSet["I_MAD"];
	nI_PDD = recordSet["I_PDD"];
	nI_MDD = recordSet["I_MDD"];
	nI_ACC = recordSet["I_ACC"];
	nI_EVA = recordSet["I_EVA"];
	nI_Speed = recordSet["I_Speed"];
	nI_Craft = recordSet["I_Craft"];
	nI_Jump = recordSet["I_Jump"];
}

void GW_ItemSlotEquip::Save(int nCharacterID)
{
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (nStatus == GW_ItemSlotStatus::DROPPED)
	{
		queryStatement << "DELETE FROM ItemSlot_EQP Where ItemSN = " << lItemSN;
		queryStatement.execute();
		return;
	}
	if (lItemSN == -1)
	{
		lItemSN = IncItemSN(GW_ItemSlotType::EQUIP);
		queryStatement << "INSERT INTO ItemSlot_EQP (ItemSN, ItemID, CharacterID, ExpireDate, Attribute, POS, RUC, CUC, I_STR, I_DEX, I_INT, I_LUK, I_MaxHP, I_MaxMP, I_PAD, I_MAD, I_PDD, I_MDD, I_ACC, I_EVA, I_Speed, I_Craft, I_Jump) VALUES("
			<< lItemSN << ", "
			<< nItemID << ", "
			<< nCharacterID << ", "
			<< lExpireDate << ", "
			<< nAttribute << ", "
			<< nPOS << ", "
			<< nRUC << ", "
			<< nCUC << ", "
			<< nI_STR << ", "
			<< nI_DEX << ", "
			<< nI_INT << ", "
			<< nI_LUK << ", "
			<< nI_MaxHP << ", "
			<< nI_MaxMP << ", "
			<< nI_PAD << ", "
			<< nI_MAD << ", "
			<< nI_PDD << ", "
			<< nI_MDD << ", "
			<< nI_ACC << ", "
			<< nI_EVA << ", "
			<< nI_Speed << ", "
			<< nI_Craft << ", "
			<< nI_Jump << ")";
	}
	else
	{
		queryStatement << "UPDATE ItemSlot_EQP Set "
			<< "ItemID = '" << nItemID << "', "
			<< "CharacterID = '" << nCharacterID << "', "
			<< "ExpireDate = '" << lExpireDate << "', "
			<< "Attribute = '" << nAttribute << "', "
			<< "POS ='" << nPOS << "', "
			<< "RUC ='" << nRUC << "', "
			<< "CUC ='" << nCUC << "', "
			<< "I_STR = '" << nI_STR << "', "
			<< "I_DEX = '" << nI_DEX << "', "
			<< "I_INT = '" << nI_INT << "', "
			<< "I_LUK = '" << nI_LUK << "', "
			<< "I_MaxHP = '" << nI_MaxHP << "', "
			<< "I_MaxMP = '" << nI_MaxMP << "', "
			<< "I_PAD = '" << nI_PAD << "', "
			<< "I_MAX = '" << nI_MAD << "', "
			<< "I_PDD = '" << nI_PDD << "', "
			<< "I_MDD = '" << nI_MDD << "', "
			<< "I_ACC = '" << nI_ACC << "', "
			<< "I_EVA = '" << nI_EVA << "', "
			<< "I_Speed = '" << nI_Speed << "', "
			<< "I_Craft = '" << nI_Craft << "', "
			<< "I_Jump = '" << nI_Jump << "' WHERE ItemSN = " << lItemSN;
	}
	queryStatement.execute();
}