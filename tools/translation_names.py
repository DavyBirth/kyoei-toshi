"""Protect approved name occurrences; coverage still requires editorial review."""
import re
from collections import Counter

def validate_names(original, translated, language, glossary, protected_literals=()):
    for entry in glossary['entries']:
        count = sum(original.count(form) for form in entry['source_forms'])
        if not count:
            continue
        spelling = entry.get(language)
        if not spelling:
            raise ValueError(f"Unverified name: {entry['id']}")
        pattern = r'(?<!\w)' + re.escape(spelling) + r'(?!\w)'
        if len(re.findall(pattern, translated)) != count:
            raise ValueError(f"Protected name differs: {entry['id']} ({language})")
    for literal in protected_literals:
        if not literal or original.count(literal) != translated.count(literal):
            raise ValueError('Player/custom name differs')
    # Percent/brace placeholders are checked separately by the draft validator.
    # Preserve engine name references even when the surrounding prose changes.
    references = re.compile(r'<[^>]+>|\[[A-Z_0-9]+\]')
    if Counter(references.findall(original)) != Counter(references.findall(translated)):
        raise ValueError('Protected engine reference differs')
