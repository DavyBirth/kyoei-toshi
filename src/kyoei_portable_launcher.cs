// Standalone launcher: relative paths, fresh local profile, no host settings.
using System;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Diagnostics;
using System.Windows.Forms;
using System.Threading.Tasks;
using System.Collections;
using System.Globalization;

class PortableLauncher : Form {
    readonly string root=AppDomain.CurrentDomain.BaseDirectory;
    readonly ComboBox language=new ComboBox(),resolution=new ComboBox();
    // Portuguese is listed only once its translation is installed.
    string[] languageCodes={"en","ja"};
    readonly CheckBox fullscreen=new CheckBox();
    readonly Button play=new Button();
    readonly Label status=new Label();
    Image artwork;
    FileStream profileLock;
    bool running;
    bool photoHudPreview;
    decimal mouseSensitivity=1.8m;
    string promptMode="auto";
    // Graphics (data/graphics.ini, read by the GPU core). The PS4 look by default.
    int anisotropy=0, sharpening=0; bool fsr1=false;
    static readonly int[] Anisotropies={0,2,4,8,16};
    static readonly int[] Sharpness={0,25,50,75};
    [STAThread] static int Main(string[] args) {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        using(var app=new PortableLauncher()) {
            if(args.Length==1 && args[0]=="--check") {
                try { app.Check(); return 0; } catch(Exception e) { Console.Error.WriteLine(e.Message);return 1; }
            }
            if(args.Length==1 && args[0]=="--render-preview") {
                app.ShowInTaskbar=false;app.Opacity=0;app.Show();Application.DoEvents();
                using(var b=new Bitmap(app.Width,app.Height)) {app.DrawToBitmap(b,new Rectangle(0,0,b.Width,b.Height));b.Save(Path.Combine(app.root,"launcher-preview.png"));}
                return 0;
            }
            if(args.Length==1 && args[0]=="--render-controls-preview") {
                using(var controls=app.ControlsDialog()) {
                    controls.ShowInTaskbar=false;controls.Opacity=0;controls.Show();Application.DoEvents();
                    using(var b=new Bitmap(controls.Width,controls.Height)){controls.DrawToBitmap(b,new Rectangle(0,0,b.Width,b.Height));b.Save(Path.Combine(app.root,"controls-preview.png"));}
                }
                return 0;
            }
            if(args.Length==1 && args[0]=="--render-graphics-preview") {
                using(var graphics=app.GraphicsDialog()) {
                    graphics.ShowInTaskbar=false;graphics.Opacity=0;graphics.Show();Application.DoEvents();
                    using(var b=new Bitmap(graphics.Width,graphics.Height)){graphics.DrawToBitmap(b,new Rectangle(0,0,b.Width,b.Height));b.Save(Path.Combine(app.root,"graphics-preview.png"));}
                }
                return 0;
            }
            if(args.Length!=0)return 2;
            Application.Run(app);return 0;
        }
    }
    PortableLauncher() {
        Text="City Shrouded in Shadow";ClientSize=new Size(980,580);
        AutoScaleMode=AutoScaleMode.None;DoubleBuffered=true;
        StartPosition=FormStartPosition.CenterScreen;FormBorderStyle=FormBorderStyle.FixedSingle;MaximizeBox=false;
        BackColor=Color.FromArgb(12,12,16);ForeColor=Color.WhiteSmoke;Font=new Font("Segoe UI",10);
        string art=Path.Combine(root,"assets","artwork.png");
        if(File.Exists(art))using(var image=Image.FromFile(art))artwork=new Bitmap(image);
        string icon=Path.Combine(root,"assets","game.ico");if(File.Exists(icon))Icon=new Icon(icon);
        AddText("巨影都市",490,38,450,60,35,Color.WhiteSmoke);
        AddText("CITY SHROUDED\nIN SHADOW",493,108,440,85,27,Color.WhiteSmoke);
        AddText("SURVIVE THE IMPOSSIBLE",496,207,430,22,10,Color.FromArgb(222,110,116));
        AddText("LANGUAGE",496,255,180,22,9,Color.Silver);
        bool portuguese=File.Exists(Path.Combine(root,"languages","pt-BR","translation.kydelta"));
        if(portuguese)languageCodes=new string[]{"en","ja","pt-BR"};
        Setup(language,496,282,430,portuguese?new string[]{"English","日本語 · Original","Português (Brasil) · prévia"}:new string[]{"English","日本語 · Original"});
        AddText("DISPLAY",496,330,180,22,9,Color.Silver);
        Setup(resolution,496,356,220,new string[]{"1280 × 720","1600 × 900","1920 × 1080","2560 × 1440","3840 × 2160"});
        fullscreen.SetBounds(735,356,200,28);fullscreen.Text="Fullscreen";fullscreen.ForeColor=Color.Silver;Controls.Add(fullscreen);
        var controlsButton=new Button();controlsButton.SetBounds(496,389,280,27);controlsButton.Text="CONTROLS & MOUSE";
        controlsButton.FlatStyle=FlatStyle.Flat;controlsButton.ForeColor=Color.WhiteSmoke;
        controlsButton.Click+=delegate{using(var dialog=ControlsDialog())dialog.ShowDialog(this);};Controls.Add(controlsButton);
        var graphicsButton=new Button();graphicsButton.SetBounds(784,389,142,27);graphicsButton.Text="GRAPHICS";
        graphicsButton.FlatStyle=FlatStyle.Flat;graphicsButton.ForeColor=Color.WhiteSmoke;
        graphicsButton.Click+=delegate{using(var dialog=GraphicsDialog())dialog.ShowDialog(this);};Controls.Add(graphicsButton);
        play.SetBounds(496,421,430,56);play.Text="START GAME";play.Font=new Font("Segoe UI",14,FontStyle.Bold);
        play.FlatStyle=FlatStyle.Flat;play.FlatAppearance.BorderSize=0;play.BackColor=Color.FromArgb(162,31,49);play.ForeColor=Color.White;
        play.Cursor=Cursors.Hand;play.Click+=delegate{Launch();};Controls.Add(play);
        status.SetBounds(496,493,430,58);status.ForeColor=Color.FromArgb(155,155,167);status.Font=new Font("Segoe UI",9);
        status.Text="English localization · work in progress\nYour saves and settings stay in this folder.";Controls.Add(status);
        LoadSettings();
        FormClosing+=delegate(object sender,FormClosingEventArgs e){if(running){e.Cancel=true;WindowState=FormWindowState.Minimized;}};
    }
    protected override void OnPaint(PaintEventArgs e) {
        base.OnPaint(e);var g=e.Graphics;
        if(artwork!=null)g.DrawImage(artwork,new Rectangle(20,45,440,440));
        using(var shade=new LinearGradientBrush(new Rectangle(20,420,440,80),Color.Transparent,BackColor,90))g.FillRectangle(shade,20,420,440,80);
        using(var pen=new Pen(Color.FromArgb(90,22,36),2))g.DrawLine(pen,470,36,470,530);
        using(var f=new Font("Segoe UI",9))using(var brush=new SolidBrush(Color.FromArgb(150,150,161)))g.DrawString("KYOEI TOSHI",f,brush,24,523);
    }
    void AddText(string text,int x,int y,int w,int h,int size,Color color) {
        var label=new Label();label.Text=text;label.SetBounds(x,y,w,h);label.Font=new Font("Segoe UI",size,FontStyle.Bold);label.ForeColor=color;Controls.Add(label);
    }
    void Setup(ComboBox c,int x,int y,int w,string[] items) {
        c.SetBounds(x,y,w,30);c.DropDownStyle=ComboBoxStyle.DropDownList;c.FlatStyle=FlatStyle.Flat;
        c.BackColor=Color.FromArgb(30,28,35);c.ForeColor=Color.WhiteSmoke;c.Items.AddRange(items);c.SelectedIndex=0;Controls.Add(c);
    }
    void Check() {
        foreach(string name in new string[]{"runtime/kyoei.exe","runtime/out/boot-linked.bin","runtime/out/imports.tsv","runtime/out/deferred.tsv","runtime/out/tls-sites.tsv","game/eboot.bin","game/sce_sys/param.sfo","game/DAT/common.psarc","assets/fonts/NotoSans-Regular.ttf","assets/fonts/NotoSansCJK-Regular.ttc","assets/hud-metrics.bin"})
            if(!File.Exists(Path.Combine(root,name)))throw new IOException("Missing portable file: "+name);
    }
    void LoadSettings() {
        try { var lines=File.ReadAllLines(Path.Combine(root,"data","settings.txt"));
            language.SelectedIndex=Math.Max(0,Array.IndexOf(languageCodes,lines[0]));
            int i;if(int.TryParse(lines[1],out i)&&i>=0&&i<resolution.Items.Count)resolution.SelectedIndex=i;
            fullscreen.Checked=lines[2]=="1";
            decimal sensitivity;if(lines.Length>3&&decimal.TryParse(lines[3],NumberStyles.Number,CultureInfo.InvariantCulture,out sensitivity))mouseSensitivity=Math.Max(.2m,Math.Min(5m,sensitivity));
            if(lines.Length>4 && (lines[4]=="controller"||lines[4]=="xbox"||lines[4]=="nintendo"))promptMode=lines[4];
            else if(lines.Length>4 && lines[4]=="keyboard-fixed")promptMode="keyboard";
            else promptMode="auto";
            photoHudPreview=lines.Length>5 && lines[5]=="1";
            int value;
            if(lines.Length>6&&int.TryParse(lines[6],out value)&&Array.IndexOf(Anisotropies,value)>=0)anisotropy=value;
            if(lines.Length>7&&int.TryParse(lines[7],out value)&&Array.IndexOf(Sharpness,value)>=0)sharpening=value;
            fsr1=lines.Length>8&&lines[8]=="1";
        }catch{}
    }
    void SaveSettings() {
        Directory.CreateDirectory(Path.Combine(root,"data"));
        File.WriteAllLines(Path.Combine(root,"data","settings.txt"),new string[]{languageCodes[language.SelectedIndex],resolution.SelectedIndex.ToString(),fullscreen.Checked?"1":"0",mouseSensitivity.ToString(CultureInfo.InvariantCulture),promptMode=="keyboard"?"keyboard-fixed":promptMode,photoHudPreview?"1":"0",
            anisotropy.ToString(CultureInfo.InvariantCulture),sharpening.ToString(CultureInfo.InvariantCulture),fsr1?"1":"0"});
    }
    // The GPU core's settings file: only the options this launcher offers.
    void WriteGraphics() {
        File.WriteAllLines(Path.Combine(root,"data","graphics.ini"),new string[]{
            "anisotropy="+anisotropy.ToString(CultureInfo.InvariantCulture),
            "rcas="+(sharpening>0?"1":"0"),"rcas_strength="+Math.Max(sharpening,25).ToString(CultureInfo.InvariantCulture),
            "fsr1="+(fsr1?"1":"0")});
    }
    Form GraphicsDialog() {
        var dialog=new Form();dialog.Text="Graphics";dialog.ClientSize=new Size(560,330);dialog.StartPosition=FormStartPosition.CenterParent;
        dialog.BackColor=BackColor;dialog.ForeColor=ForeColor;dialog.Font=Font;dialog.FormBorderStyle=FormBorderStyle.FixedDialog;dialog.MaximizeBox=false;dialog.MinimizeBox=false;
        Func<string,int,string[],int,ComboBox> row=(text,y,items,selected)=>{
            var label=new Label();label.SetBounds(18,y+4,250,25);label.Text=text;dialog.Controls.Add(label);
            var box=new ComboBox();box.SetBounds(290,y,250,28);box.DropDownStyle=ComboBoxStyle.DropDownList;box.Items.AddRange(items);box.SelectedIndex=selected;dialog.Controls.Add(box);return box;};
        var preset=row("Preset",18,new string[]{"PS4 original","Enhanced","Custom"},0);
        var af=row("Anisotropic filtering",62,new string[]{"Off (PS4)","2x","4x","8x","16x"},Array.IndexOf(Anisotropies,anisotropy));
        var sharp=row("Sharpening (AMD RCAS)",106,new string[]{"Off (PS4)","Low","Medium","High"},Array.IndexOf(Sharpness,sharpening));
        var scale=row("Scaling above 1080p",150,new string[]{"Bilinear (PS4)","AMD FSR 1"},fsr1?1:0);
        bool syncing=false;
        Action fromChoices=()=>{if(syncing)return;syncing=true;
            preset.SelectedIndex=af.SelectedIndex==0&&sharp.SelectedIndex==0&&scale.SelectedIndex==0?0:af.SelectedIndex==4&&sharp.SelectedIndex==2&&scale.SelectedIndex==1?1:2;syncing=false;};
        af.SelectedIndexChanged+=delegate{fromChoices();};sharp.SelectedIndexChanged+=delegate{fromChoices();};scale.SelectedIndexChanged+=delegate{fromChoices();};
        preset.SelectedIndexChanged+=delegate{if(syncing||preset.SelectedIndex==2)return;syncing=true;
            bool enhanced=preset.SelectedIndex==1;af.SelectedIndex=enhanced?4:0;sharp.SelectedIndex=enhanced?2:0;scale.SelectedIndex=enhanced?1:0;syncing=false;};
        fromChoices();
        var note=new Label();note.SetBounds(18,196,525,70);note.ForeColor=Color.Silver;
        note.Text="The game renders at 1920 x 1080, as on PS4. Its frame rate is limited by the processor,\nso these options change the look, not the speed. Applied when the game starts.";dialog.Controls.Add(note);
        var save=new Button();save.SetBounds(330,280,210,36);save.Text="SAVE & CLOSE";save.UseMnemonic=false;
        save.Click+=delegate{anisotropy=Anisotropies[af.SelectedIndex];sharpening=Sharpness[sharp.SelectedIndex];fsr1=scale.SelectedIndex==1;
            try{SaveSettings();dialog.Close();}catch(Exception e){MessageBox.Show(e.Message,"Settings");}};dialog.Controls.Add(save);
        return dialog;
    }
    Form ControlsDialog() {
        var dialog=new Form();dialog.Text="Controls";dialog.ClientSize=new Size(850,710);dialog.StartPosition=FormStartPosition.CenterParent;
        dialog.BackColor=BackColor;dialog.ForeColor=ForeColor;dialog.Font=Font;dialog.FormBorderStyle=FormBorderStyle.FixedDialog;dialog.MaximizeBox=false;dialog.MinimizeBox=false;
        var table=new DataGridView();table.SetBounds(16,16,818,482);table.ReadOnly=true;table.AllowUserToAddRows=false;table.AllowUserToDeleteRows=false;table.AllowUserToResizeRows=false;
        table.BackgroundColor=Color.FromArgb(24,24,30);table.BorderStyle=BorderStyle.None;table.RowHeadersVisible=false;table.AutoSizeColumnsMode=DataGridViewAutoSizeColumnsMode.Fill;
        table.DefaultCellStyle.BackColor=Color.FromArgb(30,30,38);table.DefaultCellStyle.ForeColor=Color.WhiteSmoke;table.DefaultCellStyle.SelectionBackColor=Color.FromArgb(70,42,53);table.DefaultCellStyle.SelectionForeColor=Color.White;
        table.Columns.Add("action","Action");table.Columns.Add("pc","Keyboard / mouse");table.Columns.Add("pad","PlayStation / Xbox");
        string[][] rows={
            new[]{"Move / walk / run","W A S D","Left stick"},
            new[]{"Look around","Mouse","Right stick"},
            new[]{"Interact / talk / pick up / confirm","E / Enter / left mouse","Cross / A"},
            new[]{"Back / cancel / stand up","Space / Backspace","Circle / B"},
            new[]{"Dodge / go prone","Ctrl","Square / X"},
            new[]{"Read incoming mail","F","Triangle / Y"},
            new[]{"Sprint (while moving)","Shift","R1 / RB"},
            new[]{"Camera view toward first person","Q","L1 / LB"},
            new[]{"Camera view toward third person","V","L2 / LT"},
            new[]{"Giant Shadow Camera","Right mouse / G","R2 / RT"},
            new[]{"Reset camera","Middle mouse / R","R3 / right stick click"},
            new[]{"Control guide: large / small / hidden","H","L3 / left stick click"},
            new[]{"Map","M","Touchpad / View"},
            new[]{"Drag the minimap","Hold M + mouse","Slide on touchpad / hold View + right stick"},
            new[]{"Menus / target selection","Arrow keys","D-pad"},
            new[]{"Pause / inventory menu","Esc / Tab","Options / Menu"},
            new[]{"Release / capture mouse","F7; Alt+Tab releases automatically","—"},
            new[]{"Save photo (PNG, photos folder)","F12","—"},
            new[]{"Photo mode (preview below)","F10","—"}};
        foreach(var row in rows)table.Rows.Add(row);table.ClearSelection();dialog.Controls.Add(table);
        var note=new Label();note.SetBounds(18,505,815,40);note.Text="Photo preview: WASD move; Space/Ctrl up/down; mouse or right stick looks.\nMouse wheel zooms; Shift moves faster; F10 returns to the game; F12 saves PNG.";dialog.Controls.Add(note);
        var label=new Label();label.SetBounds(18,557,350,25);label.Text="Mouse sensitivity (default 1.80)";dialog.Controls.Add(label);
        var sensitivity=new NumericUpDown();sensitivity.SetBounds(390,552,120,28);sensitivity.Minimum=.2m;sensitivity.Maximum=5m;sensitivity.DecimalPlaces=2;sensitivity.Increment=.1m;sensitivity.Value=mouseSensitivity;dialog.Controls.Add(sensitivity);
        var promptsLabel=new Label();promptsLabel.SetBounds(18,602,350,25);promptsLabel.Text="On-screen button prompts";dialog.Controls.Add(promptsLabel);
        var prompts=new ComboBox();prompts.SetBounds(390,597,210,28);prompts.DropDownStyle=ComboBoxStyle.DropDownList;prompts.Items.AddRange(new object[]{"Automatic","Keyboard & mouse","PlayStation controller","Xbox controller","Nintendo controller"});prompts.SelectedIndex=promptMode=="nintendo"?4:promptMode=="xbox"?3:promptMode=="controller"?2:promptMode=="keyboard"?1:0;dialog.Controls.Add(prompts);
        var photo=new CheckBox();photo.SetBounds(18,646,590,28);photo.Text="Photo preview: pause + free camera with F10";photo.Checked=photoHudPreview;dialog.Controls.Add(photo);
        var save=new Button();save.SetBounds(620,666,210,36);save.Text="SAVE & CLOSE";save.UseMnemonic=false;save.Click+=delegate{mouseSensitivity=sensitivity.Value;promptMode=prompts.SelectedIndex==4?"nintendo":prompts.SelectedIndex==3?"xbox":prompts.SelectedIndex==2?"controller":prompts.SelectedIndex==1?"keyboard":"auto";photoHudPreview=photo.Checked;try{SaveSettings();dialog.Close();}catch(Exception e){MessageBox.Show(e.Message,"Settings");}};dialog.Controls.Add(save);
        return dialog;
    }
    void Launch() {
        if(running)return;
        try {
            Check();
            Directory.CreateDirectory(Path.Combine(root,"data","user"));Directory.CreateDirectory(Path.Combine(root,"data","logs"));
            try {profileLock=new FileStream(Path.Combine(root,"data","session.lock"),FileMode.OpenOrCreate,FileAccess.ReadWrite,FileShare.None);}
            catch(IOException){throw new IOException("The game is already running from this folder.");}
            string lang=languageCodes[language.SelectedIndex];
            // pt-BR reuses the English executable strings and button art until its own exist.
            bool latin=lang!="ja";string shared=latin?"en":"ja";
            SaveSettings();
            var start=new ProcessStartInfo(Path.Combine(root,"runtime","kyoei.exe"),"\"runtime/out\" --app0 \"game\" --user \"data/user\" --log \"data/logs/session.log\"");
            start.WorkingDirectory=root;start.UseShellExecute=false;start.CreateNoWindow=true;start.RedirectStandardOutput=true;start.RedirectStandardError=true;
            foreach(DictionaryEntry v in Environment.GetEnvironmentVariables()) {
                string k=(string)v.Key;if(k.StartsWith("KYOEI_",StringComparison.OrdinalIgnoreCase)||k.StartsWith("GZ_",StringComparison.OrdinalIgnoreCase))start.EnvironmentVariables.Remove(k);
            }
            string[] dims=resolution.Text.Split('×');
            start.EnvironmentVariables["KYOEI_LANGUAGE"]=lang;
            start.EnvironmentVariables["KYOEI_MOUSE_SENSITIVITY"]=mouseSensitivity.ToString(CultureInfo.InvariantCulture);
            start.EnvironmentVariables["KYOEI_NAME_STORE"]="data/user/character-names.bin";
            start.EnvironmentVariables["KYOEI_TITLE_EXIT"]="1";
            start.EnvironmentVariables["KYOEI_TITLE_EXIT_ART"]="assets/title-exit.kyart";
            // Automatic follows the last input and its PlayStation, Xbox or Nintendo art.
            start.EnvironmentVariables["KYOEI_PROMPT_MODE"]=promptMode=="xbox"||promptMode=="nintendo"?"controller":promptMode;
            start.EnvironmentVariables["KYOEI_CONTROLLER_STYLE"]=promptMode=="xbox"||promptMode=="nintendo"?promptMode:promptMode=="controller"?"playstation":"auto";
            start.EnvironmentVariables["KYOEI_PHOTO_HUD"]=photoHudPreview?"1":"0";
            start.EnvironmentVariables["KYOEI_PHOTO_SCENE"]=photoHudPreview?"preview":"0";
            start.EnvironmentVariables["KYOEI_PHOTO_PHONE"]=photoHudPreview?"1":"0";
            start.EnvironmentVariables["KYOEI_PHOTO_SAVE_MARK"]=photoHudPreview?"1":"0";
            start.EnvironmentVariables["KYOEI_PHOTO_DOF"]=photoHudPreview?"1":"0";
            start.EnvironmentVariables["KYOEI_PROMPT_PACK"]=Path.Combine("languages",shared,"prompts.kypack");
            foreach(var variable in new[]{"KYOEI_ARCHIVE_DELTA","KYOEI_EBOOT_TEXT","KYOEI_TRANSLATION_ARCHIVE"})start.EnvironmentVariables.Remove(variable);
            // translation.kydelta: whole translation over the disc archive; older installs keep the 6 GB copy.
            bool wholeDelta=File.Exists(Path.Combine(root,"languages",lang,"translation.kydelta"));
            if(latin)start.EnvironmentVariables["KYOEI_ARCHIVE_DELTA"]=Path.Combine("languages",lang,wholeDelta?"translation.kydelta":"ui-corrections.kydelta");
            if(latin)start.EnvironmentVariables["KYOEI_EBOOT_TEXT"]=Path.Combine("languages",shared,"eboot-text.kytext");
            start.EnvironmentVariables["KYOEI_UI_ART"]="assets/ui-art.kyart";
            start.EnvironmentVariables["GZ_USER_NAME"]="Player";
            WriteGraphics();start.EnvironmentVariables["GOW3_CONFIG"]=Path.Combine(root,"data","graphics.ini");
            start.EnvironmentVariables["GZ_LANGUAGE"]=lang=="ja"?"0":"1";
            if(lang=="en"&&!wholeDelta)start.EnvironmentVariables["KYOEI_TRANSLATION_ARCHIVE"]="languages/en/common.psarc";
            start.EnvironmentVariables["KYOEI_HUD_FIT"]=latin?"1":"0";
            start.EnvironmentVariables["KYOEI_HUD_METRICS"]="assets/hud-metrics.bin";
            start.EnvironmentVariables["KYOEI_GUI_FONT_DIR"]="assets/fonts";
            start.EnvironmentVariables["KYOEI_IN_GAME_DIALOGS"]=latin?"1":"0";
            start.EnvironmentVariables["GZ_SYSTEM_FONT"]="assets/fonts/NotoSans-Regular.ttf";
            start.EnvironmentVariables["GZ_SYSTEM_FONT_JP"]="assets/fonts/NotoSansCJK-Regular.ttc";
            start.EnvironmentVariables["KYOEI_WINDOW_ICON"]="assets/game.ico";
            start.EnvironmentVariables["KYOEI_WINDOW_WIDTH"]=dims[0].Trim();start.EnvironmentVariables["KYOEI_WINDOW_HEIGHT"]=dims[1].Trim();
            start.EnvironmentVariables["KYOEI_FULLSCREEN"]=fullscreen.Checked?"1":"0";
            start.EnvironmentVariables["GZ_READBACK_LINEAR"]="1";start.EnvironmentVariables["GZ_PRESENT_MODE"]="Fifo";
            start.EnvironmentVariables["KYOEI_FRAME_STATS"]="1";
            start.EnvironmentVariables["KYOEI_PIPELINE_CACHE_DIR"]="data/cache/vulkan";
            running=true;play.Enabled=false;language.Enabled=false;resolution.Enabled=false;fullscreen.Enabled=false;status.Text="Starting…";
            Task.Factory.StartNew(delegate {
                int result=1;
                try {
                    using(var p=new Process()) {
                        p.StartInfo=start;p.OutputDataReceived+=delegate{};p.ErrorDataReceived+=delegate{};
                        p.Start();p.BeginOutputReadLine();p.BeginErrorReadLine();p.WaitForExit();result=p.ExitCode;
                    }
                }catch{}
                if(!IsDisposed)BeginInvoke((Action)delegate {
                    if(profileLock!=null){profileLock.Dispose();profileLock=null;}
                    running=false;play.Enabled=true;language.Enabled=true;resolution.Enabled=true;fullscreen.Enabled=true;
                    status.Text=result==0?"Game closed.":"Game stopped. See data/logs/session.log.";
                });
            });
        }catch(Exception e){if(profileLock!=null){profileLock.Dispose();profileLock=null;}MessageBox.Show(e.Message,Text,MessageBoxButtons.OK,MessageBoxIcon.Error);}
    }
    protected override void Dispose(bool disposing) {
        if(disposing){if(artwork!=null)artwork.Dispose();if(profileLock!=null)profileLock.Dispose();}
        base.Dispose(disposing);
    }
}
