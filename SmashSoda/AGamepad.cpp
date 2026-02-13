#include "AGamepad.h"

/// <summary>
/// A default AGamepad object.
/// </summary>
AGamepad::AGamepad()
	: parsec(nullptr) {
	_client = nullptr;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

/// <summary>
/// A default AGamepad object.
/// </summary>
/// <param name="parsec">ParsecDSO</param>
/// <param name="client">Driver connection object</param>
AGamepad::AGamepad(ParsecDSO* parsec, PVIGEM_CLIENT client)
	: parsec(parsec) {
	_client = client;
	_isAlive = false;
	_index = GAMEPAD_INDEX_ERROR;
	_isConnected = false;
	clearOwner();
}

/// <summary>
/// Reallocates AGamepad object.
/// </summary>
/// <returns>XBoxGamepad::alloc()</returns>
bool AGamepad::realloc() {
	release();
	return alloc();
}

/// <summary>
/// Disconnects AGamepad object.
/// </summary>
/// <returns>true if successful.</returns>
bool AGamepad::disconnect() {
	if (!_isAlive || _client == nullptr)
	{
		_isConnected = false;
		return false;
	}

	if (!VIGEM_SUCCESS(vigem_target_remove(_client, _pad)))
	{
		return false;
	}

	_index = -1;
	_isConnected = false;
	clearOwner();
	return true;
}

/// <summary>
/// Releases AGamepad object.
/// </summary>
void AGamepad::release() {
	if (_isAlive)
	{
		disconnect();
		vigem_target_free(_pad);
		clearOwner();
		_isAlive = false;
		_isConnected = false;
	}
}

/// <summary>
/// Checks to see if AGamepad is attached.
/// </summary>
/// <returns>True if attached</returns>
bool AGamepad::isAttached() {
	if (_isAlive)
	{
		return vigem_target_is_attached(_pad);
	}

	return false;
}

/// <summary>
/// Sets the index for AGamepad object.
/// </summary>
/// <param name="index"></param>
void AGamepad::setIndex(ULONG index) {
	_index = index;
}

/// <summary>
/// Gets the index for AGamepad object.
/// </summary>
/// <param name="index"></param>
ULONG AGamepad::getIndex() const {
	return _index;
}


XINPUT_STATE AGamepad::getState() {
	return _currentState;
}

AGamepad::Keyboard& AGamepad::getKeyboard()
{
	return _keyboard;
}

void AGamepad::setOwner(Guest& guest, uint32_t deviceID, bool isKeyboard)
{
	owner.guest.copy(guest);
	owner.deviceID = deviceID;
	owner.isKeyboard = isKeyboard;
}

void AGamepad::copyOwner(AGamepad* pad)
{
	if (pad != nullptr) owner.copy(pad->owner);
}

void AGamepad::clearOwner() {
	owner = GuestDevice();
}

const bool AGamepad::isOwned()
{
	return owner.guest.isValid();
}

bool AGamepad::isConnected() const
{
	return _isConnected;
}

const bool AGamepad::isLocked() const
{
	return _isLocked;
}

void AGamepad::setLocked(const bool value)
{
	_isLocked = value;
}

void AGamepad::toggleLocked()
{
	setLocked(!_isLocked);
}

const bool AGamepad::isLockedButtons() const
{
	return _isLockedButtons;
}

void AGamepad::setLockedButtons(const bool value)
{
	_isLockedButtons = value;
}

void AGamepad::toggleLockedButtons()
{
	setLockedButtons(!_isLockedButtons);
}



