from datetime import datetime,timezone,timedelta
import json
from pathlib import Path
import tempfile
import unittest
from read_codex_usage import read_usage

class UsageTests(unittest.TestCase):
    def sample(self,remaining=13,weekly=90,age=0,prefix=b'',suffix=b''):
        record=dict(type='event_msg',timestamp=(datetime.now(timezone.utc)-timedelta(seconds=age)).isoformat(),
                    payload=dict(type='token_count',rate_limits=dict(
                        primary=dict(used_percent=100-remaining,resets_at=123),
                        secondary=dict(used_percent=100-weekly,resets_at=456))))
        with tempfile.TemporaryDirectory() as temporary:
            path=Path(temporary)/'session.jsonl'
            path.write_bytes(prefix+json.dumps(record).encode()+b'\n'+suffix)
            return read_usage(path)
    def test_threshold_and_weekly(self):
        self.assertTrue(self.sample()['finish_requested'])
        self.assertFalse(self.sample(remaining=13.1)['finish_requested'])
        self.assertTrue(self.sample(remaining=90,weekly=12)['finish_requested'])
    def test_stale_and_future_are_marked(self):
        self.assertTrue(self.sample(age=601)['stale'])
        self.assertTrue(self.sample(age=-30)['stale'])
        self.assertFalse(self.sample(age=1)['stale'])
    def test_large_log_and_partial_final_write(self):
        result=self.sample(remaining=50,prefix=b'x'*(9<<20)+b'\n',suffix=b'{"unfinished":')
        self.assertEqual(result['primary_remaining'],50)
        self.assertFalse(result['finish_requested'])
    def test_no_usable_record_fails(self):
        with self.assertRaises(ValueError):self.sample(remaining=-1)

if __name__=='__main__':unittest.main()
