"""Exercise Linux loader refusal and malformed-input paths without guest code."""
import argparse,struct,subprocess,tempfile
from pathlib import Path

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('binary',type=Path);a=p.parse_args()
    binary=a.binary.resolve()
    with tempfile.TemporaryDirectory(prefix='loader-fixtures-',dir=binary.parent) as folder:
        root=Path(folder)
        def check(data,message):
            (root/'boot-linked.bin').write_bytes(data)
            result=subprocess.run([str(binary),str(root),'--list-stubs'],capture_output=True,text=True)
            assert result.returncode==1,(result.returncode,result.stdout,result.stderr)
            assert message in result.stdout,(message,result.stdout)
        result=subprocess.run([str(binary),str(root)],capture_output=True,text=True)
        assert result.returncode==2 and 'requires --list-stubs' in result.stderr
        check(b'notaboot','expected BBPROBE5')
        check(b'BBPROBE5','truncated boot file')
        check(b'BBPROBE5'+struct.pack('<6Q',0,0,1,0,1,1),'boot file limits exceeded')
        # Valid-size header followed by invalid TLS layout: reject before mapping.
        header=b'BBPROBE5'+struct.pack('<7Q',16384,0,1,0,1,1,0)
        check(header+struct.pack('<4Q',0,128,64,16),'invalid main TLS metadata')
        check(header+struct.pack('<4Q',0,0,64,3),'invalid main TLS metadata')
        check(header+struct.pack('<4Q',0,0,64,16)+struct.pack('<Q',17),'invalid module count')
    print('LINUX LOADER TESTS PASS: guest execution refused; bad magic, truncation, limits, TLS and module count rejected')

if __name__=='__main__':main()
