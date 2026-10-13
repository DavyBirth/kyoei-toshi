// Local Windows launcher. No game data is embedded or distributed.
using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Drawing;
using System.Collections.Generic;
using System.Web.Script.Serialization;
using System.Threading.Tasks;

class KyoeiLauncher {
    [STAThread]
    static int Main(string[] args) {
        bool checkOnly = args.Length == 1 && args[0] == "--check";
        bool renderOnly = args.Length == 1 && args[0] == "--render-preview";
        if (args.Length != 0 && !checkOnly && !renderOnly) return 2;
        string baseRoot = AppDomain.CurrentDomain.BaseDirectory;
        if (checkOnly) return Execute(baseRoot, true, "");
        Application.EnableVisualStyles(); Application.SetCompatibleTextRenderingDefault(false);
        using (LauncherForm form = new LauncherForm(baseRoot)) {
            if (renderOnly) {
                form.ShowInTaskbar=false; form.Opacity=0; form.Show(); Application.DoEvents();
                using (Bitmap screenshot = new Bitmap(form.Width,form.Height)) {
                    form.DrawToBitmap(screenshot,new Rectangle(Point.Empty,form.Size));
                    screenshot.Save(Path.Combine(baseRoot,"build","launcher-preview.png"));
                }
                form.Hide();
            } else Application.Run(form);
        }
        return 0;
    }
    internal static int Execute(string root, bool checkOnly, string options) {
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
                    (checkOnly ? " -CheckOnly" : "") + options);
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

class LauncherForm : Form {
    readonly string root;
    readonly ComboBox language=new ComboBox(),resolution=new ComboBox(),mode=new ComboBox();
    readonly Button play=new Button();
    readonly Label notice=new Label(),status=new Label();
    readonly string[] languages={"ja","en","pt-BR"};
    readonly string[] sizes={"1280 × 720","1600 × 900","1920 × 1080","2560 × 1440","3840 × 2160"};
    bool running;
    Color ink=Color.FromArgb(225,236,246),muted=Color.FromArgb(158,175,197),cyan=Color.FromArgb(102,225,236);
    internal LauncherForm(string root_) {
        root=root_; Text="City Shrouded in Shadow · Kyoei Toshi";
        ClientSize=new Size(980,590);FormBorderStyle=FormBorderStyle.FixedSingle;
        MaximizeBox=false;StartPosition=FormStartPosition.CenterScreen;DoubleBuffered=true;
        AutoScaleMode=AutoScaleMode.None;BackColor=Color.FromArgb(12,17,28);ForeColor=ink;
        Font=new Font("Segoe UI",10);Icon=Icon.ExtractAssociatedIcon(Path.Combine(root,"City Shrouded in Shadow.exe"));
        string artwork=Path.Combine(root,"build","launcher-assets","artwork.png");
        PictureBox picture=new PictureBox();picture.SetBounds(20,20,470,470);picture.SizeMode=PictureBoxSizeMode.Zoom;
        if(File.Exists(artwork))using(Image image=Image.FromFile(artwork))picture.Image=new Bitmap(image);
        Controls.Add(picture);
        AddLabel("SOBREVIVA À CIDADE SOB AS SOMBRAS",20,514,470,28,12,Color.FromArgb(255,118,119),FontStyle.Bold);
        AddLabel("Kyoei Toshi  ·  City Shrouded in Shadow",20,548,470,24,10,muted,FontStyle.Regular);
        AddLabel("KYOEI TOSHI  /  巨影都市",520,26,425,25,11,cyan,FontStyle.Bold);
        AddLabel("CITY SHROUDED\nIN SHADOW",517,63,440,84,27,ink,FontStyle.Bold);
        AddLabel("Seu jogo. Seu idioma. Sua história.",520,159,420,26,11,muted,FontStyle.Regular);
        AddLabel("IDIOMA",520,206,410,24,10,muted,FontStyle.Bold);
        SetupCombo(language,520,236,425,new string[]{"Japonês · original","English · prévia parcial","Português do Brasil · prévia parcial"});
        AddLabel("RESOLUÇÃO DE SAÍDA",520,283,205,24,9,muted,FontStyle.Bold);
        AddLabel("MODO DE TELA",745,283,205,24,9,muted,FontStyle.Bold);
        SetupCombo(resolution,520,313,205,sizes);SetupCombo(mode,745,313,200,new string[]{"Janela","Tela cheia sem bordas"});
        notice.SetBounds(520,352,425,55);notice.Font=new Font("Segoe UI",9);notice.ForeColor=muted;Controls.Add(notice);
        play.SetBounds(520,425,425,52);play.Text="INICIAR JOGO";play.Font=new Font("Segoe UI",13,FontStyle.Bold);
        play.BackColor=cyan;play.ForeColor=Color.FromArgb(7,22,32);play.FlatStyle=FlatStyle.Flat;play.FlatAppearance.BorderSize=0;
        play.Cursor=Cursors.Hand;Controls.Add(play);
        status.SetBounds(520,494,425,65);status.Font=new Font("Segoe UI",9);status.ForeColor=muted;Controls.Add(status);
        LoadSettings();UpdateNotice();
        language.SelectedIndexChanged+=delegate{UpdateNotice();};mode.SelectedIndexChanged+=delegate{UpdateNotice();};
        play.Click+=delegate{Launch();};
        FormClosing+=delegate(object sender,FormClosingEventArgs e){if(running){e.Cancel=true;WindowState=FormWindowState.Minimized;}else SaveSettings();};
    }
    void AddLabel(string text,int x,int y,int w,int h,int size,Color color,FontStyle style) {
        Label label=new Label();label.Text=text;label.SetBounds(x,y,w,h);label.Font=new Font("Segoe UI",size,style);label.ForeColor=color;Controls.Add(label);
    }
    void SetupCombo(ComboBox combo,int x,int y,int width,string[] items) {
        combo.SetBounds(x,y,width,32);combo.DropDownStyle=ComboBoxStyle.DropDownList;
        combo.BackColor=Color.FromArgb(30,41,58);combo.ForeColor=ink;combo.FlatStyle=FlatStyle.Flat;
        combo.Items.AddRange(items);combo.SelectedIndex=0;Controls.Add(combo);
    }
    void UpdateNotice() {
        resolution.Enabled=mode.SelectedIndex==0 && !running;
        notice.Text=language.SelectedIndex==0 ? "Texto e áudio originais em japonês." :
            "Prévia: objetivos, ações do começo e descrições das opções. Outros textos e imagens ainda em japonês.";
        status.Text=mode.SelectedIndex==1 ? "Tela cheia usa a resolução da sua tela. A tradução ainda precisa de revisão visual." :
            "A resolução de saída não altera a renderização interna. Seus saves atuais serão usados.";
    }
    void LoadSettings() {
        try {
            string path=Path.Combine(root,"build","launcher-settings.json");if(!File.Exists(path))return;
            var values=new JavaScriptSerializer().Deserialize<Dictionary<string,object>>(File.ReadAllText(path,Encoding.UTF8));
            int lang=Array.IndexOf(languages,Convert.ToString(values["language"]));language.SelectedIndex=lang<0?0:lang;
            int size=Array.IndexOf(sizes,Convert.ToString(values["resolution"]));resolution.SelectedIndex=size<0?0:size;
            mode.SelectedIndex=Convert.ToBoolean(values["fullscreen"])?1:0;
        } catch { status.Text="Preferências antigas inválidas; usando as opções padrão."; }
    }
    void SaveSettings() {
        Directory.CreateDirectory(Path.Combine(root,"build"));
        var values=new Dictionary<string,object>{{"language",languages[language.SelectedIndex]},{"resolution",sizes[resolution.SelectedIndex]},{"fullscreen",mode.SelectedIndex==1}};
        File.WriteAllText(Path.Combine(root,"build","launcher-settings.json"),new JavaScriptSerializer().Serialize(values),Encoding.UTF8);
    }
    void Launch() {
        if(running)return;SaveSettings();
        string lang=languages[language.SelectedIndex];string[] dimensions=sizes[resolution.SelectedIndex].Split('×');
        string options=" -Language "+lang+" -Width "+dimensions[0].Trim()+" -Height "+dimensions[1].Trim()+(mode.SelectedIndex==1?" -Fullscreen":"");
        running=true;play.Enabled=false;language.Enabled=false;mode.Enabled=false;resolution.Enabled=false;
        status.Text="Iniciando o jogo…";
        Task.Factory.StartNew(delegate{
            int result=KyoeiLauncher.Execute(root,false,options);
            if(!IsDisposed)BeginInvoke((Action)delegate{
                running=false;play.Enabled=true;language.Enabled=true;mode.Enabled=true;UpdateNotice();
                status.Text=result==0?"Jogo encerrado. Você pode abrir novamente.":"O jogo encerrou com erro. Veja build/launcher-last.log.";
            });
        });
    }
}
