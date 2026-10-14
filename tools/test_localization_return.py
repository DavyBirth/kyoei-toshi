"""Regression checks for protected source fields and formatting in handoffs."""
import copy
import hashlib
from audit_localization_return import audit

text='Source [i7]\n<P_NAME> [#ff44ff44]word[/#]'
entry=dict(id='p0/k1',original=text,source_text_sha256=hashlib.sha256(text.encode()).hexdigest(),en='',notes='',category='dialogue')
document=dict(format='test',title='CUSA03562',version='01.03',language='en',
              resources=[dict(path='synthetic.luc',source_sha256='resource-hash',entries=[entry])])
source={'translate-en-0001.json':document}
def altered():
    target=copy.deepcopy(source)
    e=target['translate-en-0001.json']['resources'][0]['entries'][0]
    e['en']='Draft [i7]\n<P_NAME> [#ff44ff44]text[/#]';e['notes']='Pending human review'
    return target,e

target,e=altered();assert audit(source,target)['structural_pass']
target,e=altered();e['id']='p0/k2';assert not audit(source,target)['structural_pass']
target,e=altered();e['en']=e['en'].replace('[i7]','[i8]');assert any(i['kind']=='tokens' for i in audit(source,target)['issues'])
target,e=altered();e['en']=e['en'].replace('\n',' ');assert any(i['kind']=='newlines' for i in audit(source,target)['issues'])
target,e=altered();e['source_text_sha256']='changed';assert not audit(source,target)['structural_pass']
target,e=altered();e['category']='internal';assert not audit(source,target)['structural_pass']
target,e=altered();e['en']=None;assert any(i['kind']=='value_type' for i in audit(source,target)['issues'])
assert not audit(source,{})['structural_pass']
print('LOCALIZATION AUDIT PASS: en/notes allowed; IDs, hashes, metadata, tokens, newlines and file set protected')
