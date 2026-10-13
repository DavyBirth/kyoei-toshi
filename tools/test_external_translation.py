"""Reject altered identities/tokens and keep source prose out of audited drafts."""
import copy
import unittest
from external_translation import audit_rows, digest


class HandoffAudit(unittest.TestCase):
    def setUp(self):
        self.source = 'Talk to <P_FNAME> [i4].'
        self.canonical = dict(source_sha256='resource-hash', strings=[dict(id='p0/k1', original=self.source)])
        self.resource = dict(source_sha256='resource-hash', entries=[dict(
            id='p0/k1', original=self.source, source_text_sha256=digest(self.source),
            en='Speak to <P_FNAME> [i4].', notes='Review tone.', runtime_verified=True)])
        self.glossary = dict(entries=[])

    def audit(self, resource=None, existing=()):
        return audit_rows(resource or self.resource, self.canonical, self.glossary, existing)

    def test_source_and_claims_not_copied(self):
        result = self.audit()[0]
        self.assertNotIn('original', result)
        self.assertNotIn('runtime_verified', result)
        self.assertEqual(result['editorial_status'], 'pending')

    def test_source_tampering(self):
        for field, value in [('original', 'Different'), ('source_text_sha256', 'wrong'), ('id', 'p0/k99')]:
            resource = copy.deepcopy(self.resource)
            resource['entries'][0][field] = value
            with self.assertRaises(ValueError): self.audit(resource)

    def test_duplicate_and_existing_ids(self):
        resource = copy.deepcopy(self.resource)
        resource['entries'].append(copy.deepcopy(resource['entries'][0]))
        with self.assertRaises(ValueError): self.audit(resource)
        with self.assertRaises(ValueError): self.audit(existing={'p0/k1'})

    def test_bad_resource_hash(self):
        self.resource['source_sha256'] = 'wrong'
        with self.assertRaises(ValueError): self.audit()

    def test_missing_reordered_tokens(self):
        for value in ['Speak to [i4].', '[i4] Speak to <P_FNAME>.']:
            self.resource['entries'][0]['en'] = value
            with self.assertRaises(ValueError): self.audit()

    def test_invalid_value_and_notes(self):
        for field, value in [('en', None), ('en', 'bad\0text'), ('notes', [])]:
            resource = copy.deepcopy(self.resource)
            resource['entries'][0][field] = value
            with self.assertRaises(ValueError): self.audit(resource)

    def test_uncertain_rows_remain_untranslated(self):
        self.resource['entries'][0]['en'] = ''
        self.assertEqual(self.audit(), [])

    def test_official_name_protected(self):
        self.glossary = dict(entries=[dict(id='name', source_forms=['Talk'], en='Speak')])
        self.resource['entries'][0]['en'] = 'Shout to <P_FNAME> [i4].'
        with self.assertRaises(ValueError): self.audit()


if __name__ == '__main__':
    unittest.main()
