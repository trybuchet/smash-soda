#pragma once

#include <iostream>

using namespace std;

/**
 *  A reduced data structure to hold basic guest data.
 */
class GuestData
{
public:
	/**
	 *  Empty constructor.
	 */
	GuestData()
	{
		name = "";
		userID = 0;
		reason = "";
		fake = false;
	}

	/**
	 * Full constructor.
	 * @param name Guest username.
	 * @param userID Unique user ID.
	 */
	GuestData(string name, uint32_t userID)
	{
		set(name, userID, "");
	}
	GuestData(string name, uint32_t userID, string reason)
	{
		set(name, userID, reason);
	}

	void set(string name, uint32_t userID, string reason)
	{
		this->name = name;
		this->userID = userID;
		this->reason = reason;
	}

	std::string name;
	uint32_t userID;
	std::string reason;
	bool fake;
};




