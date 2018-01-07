#include "GW_Avatar.hpp"
#include "WvsUnified.h"

#include "..\Common\Net\OutPacket.h"

void GW_Avatar::Load(int nCharacterID)
{
	//BASIC AVATAR
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "Select Hair, Face, Skin FROM CharacterStat Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	Poco::Data::RecordSet recordSet(queryStatement);

	nHair = recordSet["Hair"];
	nFace = recordSet["Face"];
	nSkin = recordSet["Skin"];

	//EQUIP
	queryStatement.reset(GET_DB_SESSION);
	queryStatement << "SELECT SN FROM ItemSlot_EQP Where CharacterID = " << nCharacterID;
	queryStatement.execute();
	recordSet.reset(queryStatement);
	GW_ItemSlotEquip eqp;
	for (int i = 0; i < recordSet.rowCount(); ++i, recordSet.moveNext())
	{
		eqp.Load(recordSet["SN"]);
		short nPos = eqp.nPOS * -1;
		if (nPos < 100)
			aHairEquip.push_back(eqp);
		else if (nPos > 100 && nPos != 111)
			aUnseenEquip.push_back(eqp);
		//There should insert totem items.
	}
}

void GW_Avatar::Save(int nCharacterID, bool newCharacter)
{
	/*Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (newCharacter)
		queryStatement << "INSERT INTO CharacterAvatar(CharacterID, Hair, Face, Skin) VALUES("
		<< nCharacterID << ","
		<< nHair << ","
		<< nFace << ","
		<< nSkin << ")";
	else

	queryStatement << "UPDATE CharacterStat SET "
	<< "Hair = '" << nHair << "',"
	<< "Face = '" << nFace << "',"
	<< "Skin = '" << nSkin << "' WHERE CharacterID = " << nCharacterID;

	queryStatement.execute();*/
}

void GW_Avatar::Encode(OutPacket *oPacket)
{
}

void GW_Avatar::Decode(InPacket * iPacket)
{
	iPacket->Decode1(); //nGender, Gender shouldn't change
	nSkin = iPacket->Decode1();
	nFace = iPacket->Decode4();
	iPacket->Decode4(); //nJob decoded at DecodeStat
	iPacket->Decode1(); //unk
	nHair = iPacket->Decode4();

	char pos = 0;
	while (1) //aHairEquip
	{
		pos = iPacket->Decode1();
		if (pos == 0xFF)
			break;
		iPacket->Decode4(); //ItemID
	}

	while (1) //aUnseenEquip
	{
		pos = iPacket->Decode1();
		if (pos == 0xFF)
			break;
		iPacket->Decode4(); //ItemID
	}

	while (1) //Unk
	{
		pos = iPacket->Decode1();
		if (pos == 0xFF)
			break;
		iPacket->Decode4(); //ItemID
	}

	while (1) //Totem
	{
		pos = iPacket->Decode1();
		if (pos == 0xFF)
			break;
		iPacket->Decode4(); //ItemID
	}

	iPacket->Decode4(); //cWeaponIdx
	iPacket->Decode4(); //?
	iPacket->Decode4(); //nSheldIdx

	iPacket->Decode1();
	iPacket->Decode4();
	iPacket->Decode4();
	iPacket->Decode4();

	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode1();
	iPacket->Decode4();
}
