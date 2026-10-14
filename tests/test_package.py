import shutil
import tempfile
import unittest
from pathlib import Path

from tools.verify_source import ROOT, check


class PackageTests(unittest.TestCase):
    def test_package_contracts(self):
        self.assertEqual(check(), [])

    def test_rpc_order_drift_is_detected(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            shutil.copytree(ROOT / "SDN_Leaderboard", root / "SDN_Leaderboard")
            server = root / "SDN_Leaderboard/scripts/5_Mission/SDN_LeaderboardServer.c"
            server.write_text(server.read_text().replace("rpc.Write(page.Request);", "rpc.Write(page.Success);", 1))
            self.assertIn("Response field order differs between writer and reader", check(root))

    def test_missing_layout_widget_is_detected(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            shutil.copytree(ROOT / "SDN_Leaderboard", root / "SDN_Leaderboard")
            layout = root / "SDN_Leaderboard/gui/layouts/SDN_Leaderboard.layout"
            layout.write_text(layout.read_text().replace("SDN_Search", "SDN_Renamed"))
            self.assertIn("Missing UI widget: SDN_Search", check(root))


if __name__ == "__main__":
    unittest.main()
