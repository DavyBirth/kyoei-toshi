from build_portable import personal_text_hits
tokens=[b'owner',b'assistant']
assert personal_text_hits(b'\0owner\0',tokens)[0]==['owner']
assert personal_text_hits(b'\x96oWnEr\xf7',tokens)==([],1)
assert personal_text_hits(b'profile: owner',tokens,True)[0]==['owner']
assert personal_text_hits(b'\0C:\\Users\\Owner\\file.pdb\0',tokens)[0]==['owner']
assert personal_text_hits('Owner'.encode('utf-16-le'),tokens)[0]
assert not personal_text_hits(b'Player',tokens)[0]
print('PORTABLE PRIVACY PASS: literal/path/UTF16 identifiers detected, compressed-byte coincidence distinguished')
