#include "GW_Avatar.hpp"
#include "WvsUnified.h"

#include "..\Common\Net\OutPacket.h"

void GW_Avatar::Load(int nCharacterID)
{
	//BASIC AVATAR
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	queryStatement << "Select c.* FROM CharacterAvatar as c Where c.CharacterID = " << nCharacterID;
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
	Poco::Data::Statement queryStatement(GET_DB_SESSION);
	if (newCharacter)
		queryStatement << "INSERT INTO CharacterAvatar(CharacterID, Hair, Face, Skin) VALUES("
		<< nCharacterID << ","
		<< nHair << ","
		<< nFace << ","
		<< nSkin << ")";
	else
		queryStatement << "UPDATE CharacterAvatar SET "
		<< "Hair = '" << nHair << "',"
		<< "Face = '" << nFace << "',"
		<< "Skin = '" << nSkin << "' WHERE CharacterID = " << nCharacterID;

	queryStatement.execute();
}

void GW_Avatar::Encode(OutPacket *oPacket)
{
}