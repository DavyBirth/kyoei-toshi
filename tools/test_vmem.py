"""Reproduce the fixed core's stale table pointer, then verify the local adapter."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
for name, defines, expected in [('reference',['-DKYOEI_VMEM_REFERENCE=1'],21),('adapted',[],0)]:
    exe=root/f'build/test-vmem-{name}.exe'
    subprocess.run(['clang','-O2','-I'+str(root/'third_party/godzilla-core/src'),
                    '-I'+str(root/'build'),*defines,str(root/'src/test_kyoei_vmem.c'),
                    '-o',str(exe)],check=True)
    result=subprocess.run([str(exe)],capture_output=True,text=True)
    print(name+': '+(result.stdout+result.stderr).strip())
    if result.returncode!=expected:
        raise SystemExit(f'{name}: expected exit {expected}, got {result.returncode}')
print('VMEM REGRESSION PASS: reference error 487 reproduced, adapted data survives fragmented reuse')
