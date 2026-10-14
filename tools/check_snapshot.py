#!/usr/bin/env python3
"""Read-only structural inspection of an SDN alpha snapshot; never repairs data."""
import argparse
import json
from pathlib import Path


def validate(data):
    def require(condition, message):
        if not condition:
            raise ValueError(message)

    def integer(record, key, low=0, high=2_000_000_000):
        value = record.get(key)
        require(type(value) is int and low <= value <= high, f"Invalid {key}")
        return value

    require(isinstance(data, dict), "Snapshot must be an object")
    require(integer(data, "SchemaVersion") == 1, "Unsupported schema")
    integer(data, "Generation", 1)
    clock = integer(data, "LastClock")
    last_event = integer(data, "LastEvent")
    for key, count, limit in (("Players", "PlayerCount", 2000), ("Pairs", "PairCount", 20000), ("Events", "EventCount", 500)):
        require(isinstance(data.get(key), list), f"Missing {key}")
        require(len(data[key]) == integer(data, count, 0, limit), f"Inconsistent {count}")
    ids = set()
    for player in data["Players"]:
        require(isinstance(player, dict), "Invalid player")
        pid = player.get("Id")
        require(isinstance(pid, str) and 0 < len(pid) <= 128 and pid not in ids, "Invalid/duplicate identity")
        ids.add(pid)
        require(isinstance(player.get("Name"), str) and len(player["Name"]) <= 48, "Invalid name")
        for key in ("Points", "ValidKills", "InvalidKills", "PvpDeaths", "UncreditedDeaths", "Streak", "PointsReachedAt", "SpawnAt"):
            integer(player, key)
        require(type(player.get("Offensive")) is bool, "Invalid Offensive flag")
        require(isinstance(player.get("Victims"), list) and len(player["Victims"]) <= 2000, "Invalid victims")
    for player in data["Players"]:
        victims = player["Victims"]
        require(all(isinstance(v, str) and v in ids and v != player["Id"] for v in victims), "Unknown victim")
        require(len(set(victims)) == len(victims), "Duplicate victim")
    pairs = set()
    for pair in data["Pairs"]:
        require(isinstance(pair, dict), "Invalid pair")
        a, b = pair.get("A"), pair.get("B")
        require(isinstance(a, str) and isinstance(b, str) and a in ids and b in ids and a != b, "Invalid pair identities")
        key = tuple(sorted((a, b)))
        require(key not in pairs, "Duplicate symmetric pair")
        pairs.add(key)
        integer(pair, "CreditedAt", 0, clock)
    previous = 0
    for event in data["Events"]:
        require(isinstance(event, dict), "Invalid event")
        previous = integer(event, "Sequence", previous + 1, last_event)
        integer(event, "At", 0, clock)
        integer(event, "Points")
        require(isinstance(event.get("Victim"), str) and event["Victim"] in ids, "Unknown event victim")
        require(isinstance(event.get("Attacker"), str) and (event["Attacker"] == "" or event["Attacker"] in ids), "Unknown attacker")
        require(isinstance(event.get("Reason"), str), "Missing event reason")
    return {"generation": data["Generation"], "players": len(ids), "pairs": len(pairs), "retained_events": len(data["Events"])}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("snapshot", type=Path)
    args = parser.parse_args()
    try:
        if args.snapshot.stat().st_size > 100_000_000:
            raise ValueError("Snapshot exceeds supported size")
        summary = validate(json.loads(args.snapshot.read_text(encoding="utf-8-sig")))
    except (OSError, ValueError, TypeError) as exc:
        parser.exit(1, f"Snapshot rejected: {exc}\n")
    print(json.dumps(summary))
    print("Structure checked only; no proof of disk durability or gameplay correctness.")


if __name__ == "__main__":
    main()
