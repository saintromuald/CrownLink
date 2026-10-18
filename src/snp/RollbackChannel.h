#pragma once

#include "shared/StormTypes.h"

// cosmo-rollback transport channel (ADR-0001 §2.1).
//
// The rollback DLL (crates/cosmo-rollback) opens this channel once the SNP is bound, by
// `GetProcAddress` on the `cl_rollback_*` exports (see CrownLink.def). Rollback turns and
// control messages travel as CrownLink custom packets with a dedicated sub-type, so they
// never reach the engine's lockstep receive queue (`spi_receive` would otherwise hand them
// to the game as if they were turns).
//
// Sub-type: storm_sdk v1.1.0's `GamePacketSubType` ends at `Messages` = 14, so 15 is free.
// It is deliberately *not* added to that enum — storm_sdk is an external FetchContent
// dependency (gitlab.com/Unveiling1561/storm_sdk) — so if storm_sdk ever allocates 15 this
// constant must move. `to_string(GamePacketSubType)` falls back to the numeric value, so
// logging still works.
//
// Peer identity: CrownLink's only peer identity is `NetAddress`, so the receive callback is
// handed the sender's `NetAddress` rather than the ADR's storm slot id. The DLL maps the
// address to a slot itself; it sees the same address on the engine's own receive path.
constexpr GamePacketSubType GamePacketSubTypeRollback = static_cast<GamePacketSubType>(15);

// `peer` is the sender's address; `data`/`len` are the rollback payload. Called on the
// juice receive thread, so the DLL must not block in it.
using RollbackReceiveCallback = void(__stdcall*)(const NetAddress* peer, const void* data, u32 len);

inline RollbackReceiveCallback g_rollback_receive_callback = nullptr;
