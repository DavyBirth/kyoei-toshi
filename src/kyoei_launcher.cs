// Local Windows launcher. No game data is embedded or distributed.
using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;

class KyoeiLauncher {
    [STAThread]
    static int Main(string[] args) {
        bool checkOnly = args.Length == 1 && args[0] == "--check";
        if (args.Length != 0 && !checkOnly) return 2;
        string root = AppDomain.CurrentDomain.BaseDirectory;
        string script = Path.Combine(root, "scripts", "run_native.ps1");
        string log = Path.Combine(root, "build", "launcher-last.log");
        bool owns = false;
        using (Mutex instance = new Mutex(false, "Local\\KyoeiToshiLocalLauncher")) {
            try {
                try { owns = instance.WaitOne(0); }
                catch (AbandonedMutexException) { owns = true; }
                if (!owns) {
                    if (!checkOnly) MessageBox.Show("O jogo ja foi aberto por este launcher.", "City Shrouded in Shadow");
                    return 3;
                }
                if (!File.Exists(script)) throw new FileNotFoundException("Launcher fora da pasta do projeto.", script);
                Directory.CreateDirectory(Path.GetDirectoryName(log));
                string powershell = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.System),
                    "WindowsPowerShell", "v1.0", "powershell.exe");
                ProcessStartInfo start = new ProcessStartInfo(powershell,
                    "-NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File \"" + script + "\"" +
                    (checkOnly ? " -CheckOnly" : ""));
                start.WorkingDirectory = root;
                start.UseShellExecute = false;
                start.CreateNoWindow = true;
                start.RedirectStandardOutput = true;
                start.RedirectStandardError = true;
                object writerLock = new object();
                using (StreamWriter writer = new StreamWriter(log, false, Encoding.UTF8)) {
                    writer.AutoFlush = true;
                    writer.WriteLine(DateTime.Now.ToString("s") + (checkOnly ? " check-only" : " user launch"));
                    using (Process child = new Process()) {
                        child.StartInfo = start;
                        DataReceivedEventHandler saveLine = delegate(object sender, DataReceivedEventArgs e) {
                            if (e.Data != null) lock (writerLock) writer.WriteLine(e.Data);
                        };
                        child.OutputDataReceived += saveLine;
                        child.ErrorDataReceived += saveLine;
                        child.Start();
                        child.BeginOutputReadLine(); child.BeginErrorReadLine();
                        child.WaitForExit();
                        int result = child.ExitCode;
                        writer.WriteLine("exit=" + result);
                        if (result != 0 && !checkOnly) MessageBox.Show(
                            "O jogo encerrou com erro. Registro salvo em:\n" + log,
                            "City Shrouded in Shadow", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return result;
                    }
                }
            } catch (Exception error) {
                if (!checkOnly) MessageBox.Show(error.Message, "City Shrouded in Shadow", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return 1;
            } finally { if (owns) instance.ReleaseMutex(); }
        }
    }
}
