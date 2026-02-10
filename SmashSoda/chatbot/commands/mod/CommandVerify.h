#pragma once

#include "Cache.h"
#include "ACommand.h"
#include "parsec-dso.h"

using namespace std;

class CommandVerify : public ACommand
{
public:

	std::string usage = "Usage: !verify <username>\nExample: !verify GreenIcicle\0";

	/**
	 * @brief Construct a new CommandBan object
	 *
	 * @param msg
	 * @param sender
	 * @param parsec
	 * @param guests
	 * @param guestHistory
	 */
	CommandVerify(const char* msg, Guest& sender, GuestList& guests, GuestDataList& guestHistory)
		: ACommand(msg, sender), guests(guests), _guestHistory(guestHistory)
	{
	}

	/**
	 * @brief Run the command
	 *
	 * @return true
	 * @return false
	 */
	bool run() override {

		// Was a guest specified?
		if (getArgs().size() == 0) {
			setReply(usage);
			return false;
		}

		// Find the guest
		if (findGuest()) {
			GuestData targetData(target.name, target.userID);
			return handleGuest(targetData, true, target.id);
		}

		// Find offline guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}
		bool found = false;
		try {
			found = _guestHistory.find(guest, [&](GuestData& guest) { _offlineGuest = guest; });
		}
		catch (const std::exception&) {}
		if (found) {
			return handleGuest(_offlineGuest, false);
		}
		return false;

	}

	/**
	 * @brief Get the prefixes object
	 *
	 * @return vector<const char*>
	 */
	static vector<const char*> prefixes() {
		return vector<const char*> { "!unban" };
	}

private:
	static vector<const char*> internalPrefixes() {
		return vector<const char*> { "!unban " };
	}

	ParsecDSO* _parsec;
	Guest target;
	GuestList guests;
	GuestDataList& _guestHistory;
	GuestData _offlineGuest;

	/**
	* Get the guest referenced in the command. Returns nullptr
	* if no guest is found
	*
	* @param guestList The guest list
	*/
	bool findGuest() {

		// Get the guest
		string guest = getArgString();
		if (guest == "") {
			return false;
		}

		try {
			uint32_t id = stoul(guest);
			vector<Guest>::iterator i;
			for (i = guests.getGuests().begin(); i != guests.getGuests().end(); ++i) {
				if ((*i).userID == id) {
					target = *i;
					return true;
				}
			}
		}
		catch (const std::exception&) {
			bool found = guests.find(guest, &target);
			if (found) {
				return true;
			}
		}

		return false;
	}

	/**
	 * @brief Handle the guest
	 *
	 * @param target
	 * @param isOnline
	 * @param guestID
	 * @return true
	 * @return false
	 */
	bool handleGuest(GuestData target, bool isOnline, uint32_t guestID = -1) {

		if (!Cache::cache.verifiedList.inList(target.userID)) {
			if (Cache::cache.verifiedList.Verify(target)) {
				setReply("Verified " + target.name + "\0");
			}
		} else {
			if (Cache::cache.verifiedList.Remove(target.userID)) {
				setReply("Unverified " + target.name + "\0");
			}
		}

		return true;
	}
};

//#pragma once
//
//#include "Cache.h"
//#include "../../../Core/Config.h"
//#include "ACommand.h"
//
//using namespace std;
//
//class CommandVerify : public ACommand
//{
//public:
//
//	/**
//	 * @brief Construct a new CommandVerify object
//	 * 
//	 * @param msg
//	 * @param sender
//	 * @param guests
//	 * @param guestHistory
//	 */
//	CommandVerify(const char* msg, Guest& sender, GuestList& guests, GuestDataList& guestHistory)
//		: ACommand(msg, sender), _sender(sender) {}
//
//	/**
//	 * @brief Run the command
//	 * 
//	 * @return true
//	 * @return false
//	 */
//	bool run() override {
//		/*ACommandSearchUserHistory::run();
//
//		bool rv = false;
//
//		switch (_searchResult)
//		{
//		case SEARCH_USER_HISTORY_RESULT::NOT_FOUND:
//			setReply(_targetUsername + " is not in the chat.\0");
//			break;
//
//		case SEARCH_USER_HISTORY_RESULT::ONLINE:
//			handleGuest(GuestData(_onlineGuest.name, _onlineGuest.userID), true, _onlineGuest.id);
//			_guestHistory.pop(_onlineGuest.userID);
//			break;
//
//		case SEARCH_USER_HISTORY_RESULT::OFFLINE:
//			handleGuest(_offlineGuest, false);
//			_guestHistory.pop(_offlineGuest.userID);
//			break;
//
//		case SEARCH_USER_HISTORY_RESULT::FAILED:
//		default:
//			setReply("Usage: !verify <User ID>\nExample: !verify 1693946\0");
//			break;
//		}*/
//
//		return false;
//	}
//
//	/**
//	 * @brief Get the prefixes object
//	 * 
//	 * @return vector<const char*>
//	 */
//	static vector<const char*> prefixes() {
//		return vector<const char*> { "!verify" };
//	}
//
//private:
//
//	/**
//	 * @brief Get the internal prefixes object
//	 * 
//	 * @return vector<const char*>
//	 */
//	static vector<const char*> internalPrefixes() {
//		return vector<const char*> { "!verify " };
//	}
//
//	Guest& _sender;
//
//	/**
//	 * @brief Handle the guest
//	 * 
//	 * @param target
//	 * @param isOnline
//	 * @param guestID
//	 * @return true
//	 * @return false
//	 */
//	bool handleGuest(GuestData target, bool isOnline, uint32_t guestID = -1) {
//		return false;
//		/*bool result = false;
//
//		if (!Cache::cache.verifiedList.inList(target.userID)) {
//
//			if (Cache::cache.verifiedList.Add(target)) {
//				setReply("Verified " + target.name + "\0");
//			} else {
//				setReply("Failed to verify " + target.name + ".\0");
//			}
//
//		}
//		else {
//
//			GuestData verifiedGuest;
//			bool found = false;
//
//			try {
//				found = Cache::cache.verifiedList.Remove(target.userID, [&verifiedGuest](GuestData& guest) {
//					verifiedGuest = guest;
//				});
//			}
//			catch (const std::exception&) { found = false; }
//
//			if (found) {
//				setReply("Unverified " + verifiedGuest.name + "\0");
//				_guestHistory.add(verifiedGuest);
//				return true;
//			}
//
//		}
//
//		return result;*/
//	}
//};


