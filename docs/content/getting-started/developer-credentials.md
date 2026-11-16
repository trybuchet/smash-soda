# Soda Arcade Developer Credentials

Smash Soda can list your Parsec room on [Soda Arcade](https://soda-arcade.com). To talk to the Soda Arcade API, every build of Smash Soda needs its own **client credentials**: a Client ID and a Client secret.

Official Smash Soda releases include credentials already. **If you build Smash Soda yourself from source, you need to create your own** and put them in `SmashSoda/PrivateConfig.h`. This page walks you through it.

!!! note "You only need this to use Soda Arcade"
    Hosting on Parsec works without credentials being valid. They are only used for the Soda Arcade features: logging in to Soda Arcade, listing your room, room snapshots and custom artwork. A build still needs *something* in the two credential fields to compile.

## 1) Become a Soda Arcade developer

1. Sign in to Soda Arcade (website or app) and finish setting up your account if it asks you to.
2. Open **Settings → Account**.
3. In the **Developer** section, click **Become a developer**.

A **Developer** page now appears in the sidebar (under **Hub**).

## 2) Create a client app

1. Open the **Developer** page from the sidebar and click **Create app**.
2. Fill in the form:

    | Field | What to enter |
    |---|---|
    | **App name** | Anything that helps you recognise it, e.g. `My Smash Soda build`. |
    | **Platform** | **Windows**. This can't be changed later. |
    | **Active** | On. Inactive apps can't connect. |
    | **Scopes** | Tick exactly the four scopes below. |

3. Tick these **scopes**. They are shown by their technical names:

    | Scope | What Smash Soda uses it for |
    |---|---|
    | `auth.login` | Logging in to Soda Arcade, and finishing first-time account setup (date of birth and accepting the Terms). |
    | `auth.session.read` | Checking your saved login when Smash Soda starts, and reading your account status. |
    | `rooms.write` | Listing, updating and closing your room, the guest count, room snapshots, and accepting the hosting rules. |
    | `media.read` | Loading your custom artwork for the room thumbnail. |

    !!! warning "Don't leave the scopes unchanged"
        If you don't tick any scopes, a new app gets a default set of *read-only* scopes, which Smash Soda can't use. You'd be able to build, but logging in to Soda Arcade would fail.

    Don't tick anything else. Smash Soda doesn't need it, and an app should only have the permissions it uses.

4. Click **Create app**.

## 3) Save your credentials

After creating the app, a **New client app credentials** window shows:

- **Client ID**: you can copy this again later from the Developer page.
- **Client secret**: **shown only once.** Copy it now and keep it somewhere safe, like a password manager.

Click **I've stored these credentials** when you're done.

If you lose the secret, select the app on the Developer page and use **Generate new credentials**. Your old credentials stop working immediately, so you'll need to rebuild with the new ones.

## 4) Add them to Smash Soda

### Option A: the setup script (recommended)

From the Smash Soda repository folder, in PowerShell:

```powershell
.\SetupPrivateConfig.ps1
```

The script:

- creates `SmashSoda/PrivateConfig.h` from `SmashSoda/PrivateConfig.example.h` if it doesn't exist yet,
- generates the three local encryption values,
- asks for your **Client ID**, then your **Client secret**. What you type for the secret is hidden.

It only asks for values that are still empty, so it's safe to run again.

### Option B: edit the file by hand

1. Copy `SmashSoda/PrivateConfig.example.h` to `SmashSoda/PrivateConfig.h`.
2. Fill in the two credential lines:

    ```cpp
    inline constexpr char ClientAppId[] = "your-client-id";
    inline constexpr char ClientAppSecret[] = "your-client-secret";
    ```

3. Fill in the three encryption values with random hexadecimal text (characters `0-9` and `a-f`). They protect the login data Smash Soda saves on your PC:

    | Setting | Length |
    |---|---|
    | `SessionEncryptionKey` | 64 characters |
    | `SessionEncryptionNonce` | 24 characters |
    | `ArcadeCredentialsEncryptionKey` | 64 characters |

    The easiest way to get these is to run `SetupPrivateConfig.ps1` anyway: it fills in empty encryption values and keeps everything you've already entered.

### API settings

The rest of `PrivateConfig.h` tells Smash Soda where Soda Arcade is. For the live service, keep the defaults:

```cpp
inline constexpr char ApiDomain[] = "soda-arcade.com";
inline constexpr unsigned short ApiPort = 0;
inline constexpr bool ApiSecure = true;
inline constexpr char ApiBasePath[] = "/api";
```

`ApiDomain` is the host only: no `https://` and no path.

## 5) Build and check it works

Build as described in [Manual Build (Windows)](manual-build.md):

```powershell
cmake -S . -B build
cmake --build build --config Release --parallel
```

The build stops with a "Set ... in SmashSoda/PrivateConfig.h" error if any value is still empty.

Then, in Smash Soda:

1. Log in to Soda Arcade when asked, or untick **Private Room** in Host Settings.
2. If your account is new, Smash Soda asks for your date of birth and for you to accept the Terms of Service and Privacy Policy. This is the same one-time setup as in the Soda Arcade app.
3. The first time you list a room, Smash Soda shows the hosting rules. Accept them to list your room.
4. Start hosting. The log should say **Your room has been posted on https://soda-arcade.com**.

## Troubleshooting

Errors from Soda Arcade appear in Smash Soda's **Log** window, or in the Soda Arcade login window when you log in.

| Message | What it means | Fix |
|---|---|---|
| `api.errors.client_app.missing_credentials` | The build has no Client ID or secret. | Fill in `ClientAppId` and `ClientAppSecret`, then rebuild. |
| `api.errors.client_app.invalid_credentials` | The ID or secret is wrong, the app is inactive, or the credentials were regenerated. | Check both values and that the app is **Active**. If you regenerated them, rebuild with the new ones. |
| `api.errors.client_app.missing_scope` | Your app is missing a scope Smash Soda needs. | On the Developer page, edit the app and tick all four scopes from step 2. No rebuild needed. |
| `api.errors.unauthorized_endpoint` | Smash Soda tried something third-party apps aren't allowed to do. | Make sure you're on an up-to-date version of Smash Soda. |
| "Finish setting up your Soda Arcade account…" | Your account still needs the one-time setup. | Complete the account setup window. |
| "Please read and accept the Soda Arcade hosting rules…" | You haven't accepted the current hosting rules. | Accept them in the window that appears. |

## Keep your credentials safe

- **Never commit `PrivateConfig.h`.** It's already in `.gitignore`. Double-check before pushing to a fork.
- **Your secret is compiled into `SmashSoda.exe`.** If you share your build with other people, they can extract your credentials. If you distribute builds, keep them to people you trust, and regenerate your credentials if they leak.
- **People log in to Soda Arcade with their own password in your build.** Only use and share builds you trust. Soda Arcade can deactivate apps that misuse their access.
- You can have up to **three** client apps. Delete ones you no longer use from the Developer page.
