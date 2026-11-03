import datetime as dt
import json
from pathlib import Path
import tempfile
import unittest
from watch_quota import ROOT,quota_state,exhausted

class Quota(unittest.TestCase):
    def read(self,primary=93,weekly=94,age=0,parent=None,cwd=None,reset=300):
        now=dt.datetime.now(dt.timezone.utc)
        metadata=dict(id='main',cwd=str(cwd or ROOT),parent_thread_id=parent)
        record=dict(timestamp=(now-dt.timedelta(seconds=age)).isoformat(),payload=dict(rate_limits=dict(
            limit_id='codex',primary=dict(used_percent=primary,window_minutes=300,resets_at=now.timestamp()+reset),
            secondary=dict(used_percent=weekly,window_minutes=10080,resets_at=now.timestamp()+reset))))
        with tempfile.TemporaryDirectory() as folder:
            file=Path(folder)/'session.jsonl'
            file.write_text(json.dumps(dict(payload=metadata))+'\n'+json.dumps(record)+'\n',encoding='utf8')
            return quota_state(file,'main',now)
    def test_either_real_window_at_five_triggers(self):
        self.assertEqual(exhausted(self.read(94,95)),['weekly'])
        self.assertEqual(exhausted(self.read(95,94)),['five_hour'])
        self.assertEqual(exhausted(self.read(94,94)),[])
        self.assertEqual(exhausted(self.read(100,100)),['five_hour','weekly'])
    def test_stale_reset_or_invalid_values_cannot_trigger(self):
        for state in (self.read(age=901),self.read(age=-1),self.read(reset=-1),self.read(float('nan'),float('nan'))):
            self.assertEqual(exhausted(state),[])
    def test_other_conversations_or_workspaces_are_rejected(self):
        with self.assertRaises(ValueError):self.read(parent='review-child')
        with self.assertRaises(ValueError):self.read(cwd=ROOT.parent)

if __name__=='__main__':unittest.main()
