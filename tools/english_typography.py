"""Use proportional Latin punctuation supported by both native font tables."""
def native_english(text):
    return text.translate(str.maketrans({'\u2018': "'", '\u2019': "'",
                                        '\u201c': '"', '\u201d': '"'}))
