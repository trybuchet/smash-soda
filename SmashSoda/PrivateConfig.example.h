#pragma once

// Copy this file to PrivateConfig.h and replace the empty values before building.
// PrivateConfig.h is ignored by Git and compiled into the local executable.
namespace SmashSodaBuildConfig {
    inline constexpr char Version[] = "7.0.4";

    // The HTTP library receives these as separate host, port, secure, and route values.
    inline constexpr char ApiDomain[] = "soda-arcade.com";
    inline constexpr unsigned short ApiPort = 0;
    inline constexpr bool ApiSecure = true;
    inline constexpr char ApiBasePath[] = "/api";

    // Supply your own Soda Arcade API credentials and local encryption values.
    inline constexpr char ClientAppId[] = "";
    inline constexpr char ClientAppSecret[] = "";
    inline constexpr char SessionEncryptionKey[] = "";
    inline constexpr char SessionEncryptionNonce[] = "";
    inline constexpr char ArcadeCredentialsEncryptionKey[] = "";
}
