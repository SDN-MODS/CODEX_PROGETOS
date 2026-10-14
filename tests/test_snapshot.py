import copy
import unittest

from tools.check_snapshot import validate


def player(pid):
    return dict(Id=pid, Name=pid, Points=0, ValidKills=0, InvalidKills=0,
                PvpDeaths=0, UncreditedDeaths=0, Streak=0,
                PointsReachedAt=0, SpawnAt=0, Offensive=False, Victims=[])


def snapshot():
    return dict(SchemaVersion=1, Generation=1, LastClock=100, LastEvent=1,
                PlayerCount=2, PairCount=1, EventCount=1,
                Players=[player("A"), player("B")],
                Pairs=[dict(A="A", B="B", CreditedAt=99)],
                Events=[dict(Sequence=1, At=100, Victim="B", Attacker="A", Reason="kill_direta_valida", Points=100)])


class SnapshotInspectionTests(unittest.TestCase):
    def test_valid_snapshot_is_read_only(self):
        data = snapshot()
        original = copy.deepcopy(data)
        self.assertEqual(validate(data)["players"], 2)
        self.assertEqual(data, original)

    def test_reverse_pair_duplicate_is_rejected(self):
        data = snapshot()
        data["Pairs"].append(dict(A="B", B="A", CreditedAt=100))
        data["PairCount"] = 2
        with self.assertRaisesRegex(ValueError, "symmetric"):
            validate(data)

    def test_future_pair_is_rejected(self):
        data = snapshot()
        data["Pairs"][0]["CreditedAt"] = 101
        with self.assertRaisesRegex(ValueError, "CreditedAt"):
            validate(data)

    def test_unknown_victim_is_rejected(self):
        data = snapshot()
        data["Players"][0]["Victims"] = ["unknown"]
        with self.assertRaisesRegex(ValueError, "victim"):
            validate(data)

    def test_duplicate_identity_is_rejected(self):
        data = snapshot()
        data["Players"][1]["Id"] = "A"
        with self.assertRaisesRegex(ValueError, "identity"):
            validate(data)

    def test_missing_fields_are_not_defaults(self):
        data = snapshot()
        del data["PlayerCount"]
        with self.assertRaisesRegex(ValueError, "PlayerCount"):
            validate(data)

    def test_schema_upgrade_requires_migration(self):
        data = snapshot()
        data["SchemaVersion"] = 2
        with self.assertRaisesRegex(ValueError, "schema"):
            validate(data)

    def test_boolean_is_not_valid_integer(self):
        data = snapshot()
        data["Players"][0]["Points"] = True
        with self.assertRaisesRegex(ValueError, "Points"):
            validate(data)

    def test_replayed_sequence_is_rejected(self):
        data = snapshot()
        data["Events"].append(copy.deepcopy(data["Events"][0]))
        data["EventCount"] = 2
        with self.assertRaisesRegex(ValueError, "Sequence"):
            validate(data)

    def test_count_mismatch_is_rejected(self):
        data = snapshot()
        data["PlayerCount"] = 1
        with self.assertRaisesRegex(ValueError, "PlayerCount"):
            validate(data)


if __name__ == "__main__":
    unittest.main()
