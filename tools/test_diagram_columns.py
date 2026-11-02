import unittest
from make_pad_diagrams import improve_columns

class Columns(unittest.TestCase):
    def test_accepted_move_is_not_repeated_from_previous_owner(self):
        first=dict(below=False,entries=[{'id':'a'},{'id':'b'}])
        second=dict(below=False,entries=[]);third=dict(below=False,entries=[])
        columns=[first,second,third]
        def solve(column):return len(column['entries']) if column is first else 0
        scores={id(c):solve(c) for c in columns}
        improve_columns(columns,scores,solve)
        self.assertEqual(first['entries'],[])
        self.assertEqual([e['id'] for c in columns for e in c['entries']],['a','b'])
    def test_rejected_moves_restore_both_owners(self):
        a=dict(below=False,entries=[{'id':'a'}]);b=dict(below=False,entries=[{'id':'b'}])
        columns=[a,b];scores={id(c):1 for c in columns}
        improve_columns(columns,scores,lambda c:1)
        self.assertEqual(a['entries'],[{'id':'a'}]);self.assertEqual(b['entries'],[{'id':'b'}])

if __name__=='__main__':unittest.main()
