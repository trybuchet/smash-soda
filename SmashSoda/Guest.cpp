#include "Guest.h"

Guest::Guest()
	: name(""), userID(0), id(0), status(Status::INVALID), fake(false), spectator(false)
{}

Guest::Guest(string name, uint32_t userID, uint32_t id, Status status, bool fake, bool spectator)
	: name(name), userID(userID), id(id), status(status), fake(fake), spectator(false)
{}

Guest::Guest(ParsecGuest guest)
	: name(guest.name), userID(guest.userID), id(guest.id), status(Status::OK), fake(false), spectator(false)
{}

const bool Guest::isValid()
{
	return status == Status::OK;
}

const bool Guest::isFake()
{
	return fake;
}

Guest Guest::copy(const Guest& guest)
{
	name = guest.name;
	userID = guest.userID;
	id = guest.id;
	status = guest.status;
	fake = guest.fake;
	spectator = guest.spectator;
	return *this;
}


