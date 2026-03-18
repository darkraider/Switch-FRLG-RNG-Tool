const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <title>Switch FR/LG RNG Tool</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    :root { --bg: #1e1e2e; --surface: #2a2a3f; --primary: #89b4fa; --danger: #f38ba8; --text: #cdd6f4; --success: #a6e3a1; --warn: #fab387; --btn-dark: #313244; }
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: var(--bg); color: var(--text); text-align: center; margin: 0; padding: 20px; }
    .container { max-width: 800px; margin: auto; background: var(--surface); padding: 20px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }
    h1, h2, h3 { color: var(--primary); margin-top: 0; }
    
    .section { background: #11111b; padding: 20px; border-radius: 12px; margin-bottom: 20px; border: 1px solid #313244; box-shadow: inset 0 2px 10px rgba(0,0,0,0.2); }
    
    .timer-display { background: #11111b; padding: 25px 20px 15px; border-radius: 8px; margin: 20px 0; transition: background 0.05s, color 0.05s; }
    .timer-display.flash-active { background: var(--primary) !important; color: #11111b !important; }
    .stage-label { font-size: 1.2rem; margin-bottom: 5px; color: #a6adc8; font-weight: bold;}
    #timeRemaining { font-size: 4.5rem; font-family: monospace; color: var(--success); font-weight: bold; display: block; line-height: 1; margin-bottom: 15px;}
    .flash-active #timeRemaining { color: #11111b !important; }

    .stats-row { display: flex; justify-content: space-around; margin-top: 15px; border-top: 2px solid #313244; padding-top: 15px; }
    .stat-title { color: #bac2de; font-size: 0.9rem; font-family: 'Segoe UI', sans-serif;}
    .stat-value { font-family: monospace; font-size: 1.1rem; color: #cdd6f4; display: block; margin-top: 5px;}
    
    .btn-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; margin-bottom: 20px; }
    .btn { background-color: var(--primary); color: #11111b; font-weight: bold; border: none; padding: 12px; width: 100%; border-radius: 8px; font-size: 1rem; cursor: pointer; transition: 0.2s; }
    .btn:hover { filter: brightness(1.2); }
    .btn:disabled { background-color: #45475a; cursor: not-allowed; color: #a6adc8; }
    .btn-danger { background-color: var(--danger); }
    .btn-warn { background-color: var(--warn); }
    
    .sequence-header { display: flex; flex-direction: column; align-items: flex-start; margin-bottom: 15px; border-bottom: 1px solid #45475a; padding-bottom: 15px; gap: 15px;}
    .profile-controls { width: 100%; display: flex; gap: 8px; align-items: center; justify-content: flex-start; flex-wrap: wrap;}
    #profileSelect { margin-left: auto; } 
    
    .profile-controls button { padding: 6px 10px !important; font-size: 0.85rem !important; }
    .profile-controls select, .profile-controls input { padding: 6px; border-radius: 4px; border: 1px solid #45475a; background: var(--btn-dark); color: white; font-size: 0.85rem;}
    #profileName { width: 120px; }
    
    #timeline { display: flex; flex-direction: column; gap: 15px; margin-bottom: 20px; text-align: left; }
    
    .settings-panel { background: #11111b; border-radius: 12px; margin-bottom: 20px; padding: 15px 20px; border: 1px solid #313244; transition: 0.2s; text-align: left; box-shadow: inset 0 2px 10px rgba(0,0,0,0.2); }
    .settings-panel summary { color: #bac2de; font-weight: bold; cursor: pointer; outline: none; list-style: none; display: flex; justify-content: space-between; align-items: center; font-size: 0.95rem; }
    .settings-panel summary::-webkit-details-marker { display: none; }
    .settings-panel summary::after { content: '▼'; font-size: 0.8rem; color: #a6adc8; }
    .settings-panel[open] summary::after { content: '▲'; }
    .settings-content { margin-top: 15px; padding-top: 15px; border-top: 1px solid #313244; display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 15px; }

    .active-block { border-left-color: var(--success) !important; background-color: rgba(166, 227, 161, 0.15) !important; box-shadow: 0 0 12px rgba(166, 227, 161, 0.25); border-radius: 4px; }
    
    .action-row { display: flex; align-items: center; gap: 10px; background: #181825; padding: 12px; border-radius: 8px; border-left: 4px solid #a6adc8; animation: slideIn 0.2s ease-out; flex-wrap: wrap; transition: opacity 0.2s, background-color 0.2s;}
    .phase-container { background: #181825; border-radius: 8px; border-left: 4px solid #a6adc8; padding: 15px; animation: slideIn 0.2s ease-out; position: relative; transition: opacity 0.2s, background-color 0.2s;}
    .phase-header { font-weight: bold; color: var(--primary); margin-bottom: 15px; display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #313244; padding: 8px; transition: 0.2s; border-left: 4px solid transparent;}
    
    .p-name-input { background: transparent; border: none; color: var(--primary); font-weight: bold; font-size: 1rem; outline: none; border-bottom: 1px dashed transparent; width: 220px; font-family: inherit; transition: border-color 0.2s;}
    .p-name-input:focus, .p-name-input:hover { border-bottom-color: #45475a; }
    .p-name-input::placeholder { color: #45475a; }

    .phase-master-timer { display: flex; gap: 15px; margin-bottom: 15px; align-items: flex-end; flex-wrap: wrap;}
    .phase-tasks { background: #11111b; padding: 10px; border-radius: 8px; margin-bottom: 15px;}
    .phase-tasks-title { font-size: 0.8rem; color: #bac2de; text-transform: uppercase; margin-bottom: 10px; font-weight: bold; display: flex; justify-content: space-between; align-items: center;}
    .task-row { display: flex; gap: 10px; align-items: center; margin-bottom: 8px; padding: 8px; border-bottom: 1px dashed #313244; border-left: 4px solid transparent; transition: 0.2s;}
    .task-row:last-child { border-bottom: none; margin-bottom: 0;}
    .phase-trigger { display: flex; gap: 10px; align-items: center; background: #313244; padding: 10px; border-radius: 8px; border-left: 4px solid transparent; transition: 0.2s;}
    
    @keyframes slideIn { from { opacity: 0; transform: translateY(-10px); } to { opacity: 1; transform: translateY(0); } }
    
    select, input[type="number"] { padding: 8px; border-radius: 4px; border: 1px solid #45475a; background: var(--btn-dark); color: white; font-size: 0.9rem;}
    input[type="number"] { width: 80px; }
    label { margin: 0 5px 0 0; font-size: 0.8rem; color: #bac2de; display: flex; align-items: center; margin-bottom: 3px;}
    
    .block-controls { display: flex; gap: 5px; margin-left: auto; align-items: center; }
    .move-btn { background: #313244; color: #cdd6f4; border: none; border-radius: 4px; width: 28px; height: 28px; font-weight: bold; cursor: pointer; display: flex; align-items: center; justify-content: center;}
    .del-btn { background: var(--danger); color: #11111b; border: none; border-radius: 4px; width: 28px; height: 28px; font-weight: bold; cursor: pointer; display: flex; align-items: center; justify-content: center;}
    .del-task-btn { background: transparent; color: var(--danger); border: none; font-weight: bold; cursor: pointer; font-size: 1.2rem; margin-left: auto; }
    
    .add-btns { display: flex; gap: 10px; }
    .add-action-btn { background: transparent; border: 2px dashed #45475a; color: #a6adc8; width: 100%; padding: 12px; border-radius: 8px; font-weight: bold; cursor: pointer; transition: 0.2s;}
    .add-phase-btn { background: transparent; border: 2px dashed var(--primary); color: var(--primary); width: 100%; padding: 12px; border-radius: 8px; font-weight: bold; cursor: pointer; transition: 0.2s;}
    .add-task-btn { background: #313244; color: #cdd6f4; border: none; padding: 4px 8px; border-radius: 4px; font-size: 0.75rem; cursor: pointer; font-weight: bold;}
    .calib-btn { background: var(--warn); color: #11111b; border: none; border-radius: 4px; padding: 8px 12px; font-weight: bold; cursor: pointer; font-size: 0.8rem; margin-top: 18px;}

    .tooltip { display: inline-flex; justify-content: center; align-items: center; margin-left: 6px; cursor: help; color: var(--primary); font-weight: bold; border-radius: 50%; width: 16px; height: 16px; font-size: 0.7rem; background: rgba(137, 180, 250, 0.15); position: relative; font-style: normal; }
    .tooltip::after { content: attr(data-tip); position: absolute; bottom: 150%; left: 50%; transform: translateX(-50%); background: #1e1e2e; border: 1px solid var(--primary); padding: 8px 12px; border-radius: 6px; color: #cdd6f4; font-size: 0.8rem; font-weight: normal; white-space: normal; width: max-content; max-width: 250px; opacity: 0; pointer-events: none; transition: opacity 0.2s, bottom 0.2s; z-index: 100; box-shadow: 0 4px 15px rgba(0,0,0,0.6); text-align: left; line-height: 1.4;}
    .tooltip::before { content: ''; position: absolute; bottom: 100%; left: 50%; transform: translateX(-50%); border-width: 6px; border-style: solid; border-color: var(--primary) transparent transparent transparent; opacity: 0; transition: opacity 0.2s, bottom 0.2s; pointer-events: none;}
    .tooltip:hover::after { opacity: 1; bottom: 130%; }
    .tooltip:hover::before { opacity: 1; bottom: calc(130% - 12px); }

    .status-text { color: #a6adc8; font-size: 1rem; margin-top: 25px; font-weight: bold; }
    .gamepad-container { display: flex; justify-content: space-around; align-items: center; background: #11111b; padding: 25px 20px; border-radius: 12px; margin-top: 10px; box-shadow: inset 0 2px 10px rgba(0,0,0,0.5); }
    
    .dpad-container { display: grid; grid-template-columns: 45px 45px 45px; grid-template-rows: 45px 45px 45px; filter: drop-shadow(0px 4px 0px #1e1e2e); }
    .dbtn { width: 100%; height: 100%; background: #313244; color: #cdd6f4; font-weight: bold; font-size: 1.2rem; border: none; cursor: pointer; touch-action: none; user-select: none; transition: 0.1s; display: flex; align-items: center; justify-content: center; padding: 0; margin: 0;}
    .dbtn:active, .dbtn.pressed { background: var(--primary); color: #11111b; transform: translateY(4px); }
    .d-up { grid-column: 2; grid-row: 1; border-radius: 8px 8px 0 0; }
    .d-left { grid-column: 1; grid-row: 2; border-radius: 8px 0 0 8px; }
    .d-right { grid-column: 3; grid-row: 2; border-radius: 0 8px 8px 0; }
    .d-down { grid-column: 2; grid-row: 3; border-radius: 0 0 8px 8px; }
    .d-center { grid-column: 2; grid-row: 2; background: #313244; pointer-events: none; position: relative;}
    .d-center::after { content: ''; position: absolute; top: 30%; left: 30%; right: 30%; bottom: 30%; background: #1e1e2e; border-radius: 50%; opacity: 0.5;}

    .center-col { display: flex; flex-direction: column; align-items: center; gap: 15px; }
    .plus-btn { width: 45px; height: 45px; border-radius: 50%; background: #bac2de; color: #11111b; font-size: 1.5rem; font-weight: bold; border: none; touch-action: none; user-select: none; cursor: pointer; box-shadow: 0 4px 0 #8b92aa; transition: 0.1s;}
    .home-btn { width: 65px; height: 35px; border-radius: 8px; background: #a6adc8; color: #11111b; font-size: 0.85rem; font-weight: bold; border: none; touch-action: none; user-select: none; cursor: pointer; box-shadow: 0 4px 0 #7b839b; transition: 0.1s;}
    .plus-btn:active, .plus-btn.pressed, .home-btn:active, .home-btn.pressed { transform: translateY(4px); box-shadow: none !important; filter: brightness(1.2); }

    .action-diamond { display: grid; grid-template-columns: 50px 50px 50px; grid-template-rows: 50px 50px 50px; justify-items: center; align-items: center; gap: 5px; }
    .abtn { width: 65px; height: 65px; border-radius: 50%; border: none; color: white; font-weight: bold; font-size: 1.5rem; cursor: pointer; touch-action: none; user-select: none; transition: 0.1s;}
    .abtn:active, .abtn.pressed { transform: translateY(4px); box-shadow: none !important; filter: brightness(1.2); }
    .btn-x { grid-column: 2; grid-row: 1; background: #cba6f7; box-shadow: 0 4px 0 #7e60a3; }
    .btn-y { grid-column: 1; grid-row: 2; background: #a6e3a1; box-shadow: 0 4px 0 #5b9e57; color: #11111b; }
    .btn-a { grid-column: 3; grid-row: 2; background: #f38ba8; box-shadow: 0 4px 0 #8b0000; }
    .btn-b { grid-column: 2; grid-row: 3; background: #fab387; box-shadow: 0 4px 0 #b85e00; color: #11111b; }

    @media (max-width: 600px) {
      body { padding: 10px; }
      .container { padding: 15px; }
      h1 { font-size: 1.6rem; }
      #timeRemaining { font-size: 3.5rem; }
      
      .btn-grid { grid-template-columns: 1fr 1fr; grid-auto-flow: dense; }
      #startBtn { grid-column: 1; grid-row: 1; }
      #stopBtn { grid-column: 2; grid-row: 1; margin: 0; }
      #restartBtn { grid-column: 1 / -1; grid-row: 2; margin-top: 0; }
      
      .settings-content { grid-template-columns: 1fr; }
      
      .sequence-header { flex-direction: column; align-items: flex-start; }
      #profileSelect { margin-left: 0; } 
      .profile-controls { width: 100%; display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
      .profile-controls input[type="text"], .profile-controls select { grid-column: span 2; width: 100% !important; box-sizing: border-box; padding: 10px !important; font-size: 0.9rem !important;}
      .profile-controls button { width: 100% !important; padding: 10px !important; margin: 0; justify-content: center; font-size: 0.9rem !important;}
      
      .action-row { flex-direction: column; align-items: flex-start; gap: 10px; padding-bottom: 15px;}
      .action-row > select { width: 100% !important; }
      .action-row > div[id$='_inputs'] { width: 100%; display: flex; gap: 10px; }
      .action-row > div[id$='_inputs'] > div { flex: 1; display: flex; flex-direction: column; }
      .action-row > div[id$='_inputs'] select, .action-row > div[id$='_inputs'] input { width: 100% !important; box-sizing: border-box; }
      .action-row .block-controls { width: 100%; justify-content: flex-end; margin-top: 5px; }
      
      .phase-header { flex-direction: column; align-items: flex-start; gap: 10px; }
      .phase-header > div:first-child { width: 100%; }
      .p-name-input { width: calc(100% - 30px) !important; }
      .phase-header .block-controls { width: 100%; justify-content: flex-end; }
      
      .phase-master-timer { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; align-items: end; }
      .phase-master-timer > div { width: 100%; display: flex; flex-direction: column; }
      .phase-master-timer select, .phase-master-timer input { width: 100% !important; box-sizing: border-box; }
      .calib-btn { grid-column: span 2; width: 100%; margin-top: 5px; }
      
      .phase-tasks-title { flex-direction: column; align-items: flex-start; gap: 10px; }
      .add-task-btn { width: 100%; padding: 10px; }
      
      .task-row { flex-wrap: wrap; position: relative; padding-top: 35px; background: #181825; border-radius: 8px;}
      .task-row .t-type { width: 100% !important; }
      .task-row > div[id$='_inputs'] { width: 100%; display: flex; gap: 10px; }
      .task-row > div[id$='_inputs'] select, .task-row > div[id$='_inputs'] input { flex: 1; width: 100% !important; box-sizing: border-box; }
      .del-task-btn { position: absolute; top: 5px; right: 5px; margin: 0; background: #313244; border-radius: 4px; padding: 2px 10px; }
      
      .phase-trigger { flex-direction: column; align-items: flex-start; gap: 10px; }
      .phase-trigger > span { width: 100% !important; }
      .phase-trigger .p-trig-type { width: 100% !important; }
      .phase-trigger > div[id$='_trig_inputs'] { width: 100%; display: flex !important; gap: 10px; }
      .phase-trigger > div[id$='_trig_inputs'] select, .phase-trigger > div[id$='_trig_inputs'] input { flex: 1; width: 100% !important; box-sizing: border-box; }
      
      .gamepad-container { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; justify-items: center; align-items: center; gap: 15px; padding: 20px 5px; }
      .dpad-container { grid-column: 1; grid-row: 1; transform: scale(0.85); margin: 0; }
      .action-diamond { grid-column: 2; grid-row: 1; transform: scale(0.85); margin: 0; }
      .center-col { grid-column: 1 / -1; grid-row: 2; flex-direction: row; gap: 30px; margin-top: 0; }
      
      .tooltip::after { left: auto; right: -10px; transform: none; width: 220px; }
      .tooltip::before { left: auto; right: 10px; transform: none; }
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Switch FR/LG RNG Tool</h1>
    
    <div class="timer-display" id="timerContainer">
      <div class="stage-label" id="stageLabel">IDLE</div>
      <span id="timeRemaining">0.000</span>
      <div class="stats-row">
        <div><span class="stat-title">Phase:</span><span class="stat-value" id="phaseDisplay">-</span></div>
        <div><span class="stat-title">Next:</span><span class="stat-value" id="nextDisplay">-</span></div>
        <div><span class="stat-title">Total:</span><span class="stat-value" id="totalDisplay">-</span></div>
      </div>
    </div>

    <div class="btn-grid">
      <button id="startBtn" class="btn" onclick="triggerRun()">Start</button>
      <button id="restartBtn" class="btn btn-warn" onclick="restartMacro()">Restart (R or ☰)</button>
      <button id="stopBtn" class="btn btn-danger" onclick="stopMacro()" disabled>ABORT</button>
    </div>

    <div style="text-align: left; margin-bottom: 20px;">
      <label style="font-size: 1rem; display: inline-flex; align-items: center;"><input type="checkbox" id="soundToggle" checked style="margin-right: 8px;"> Enable Audio Beeps & Flash</label>
    </div>

    <details class="settings-panel">
      <summary>⚙️ Hardware Boot Settings <i class="tooltip" style="margin-left:auto; margin-right: 15px;" data-tip="These delays control the automatic startup sequence before your timeline begins.">i</i></summary>
      <div class="settings-content">
        <div>
           <div style="display:flex; align-items:center; margin-bottom:3px;">
             <input type="checkbox" id="cfg_boot1_en" checked onchange="saveBootSettings()" style="margin: 0 5px 0 0; width:auto;">
             <label style="margin:0;">Profile Dialog (ms) <i class="tooltip" data-tip="Time needed for the Switch to open the 'Who is playing?' screen. If you are the only profile on the console, uncheck this box.">i</i></label>
           </div>
           <input type="number" id="cfg_boot1" value="1000" onchange="saveBootSettings()">
        </div>
        <div><label>App Launch Wait (ms) <i class="tooltip" data-tip="Time spent between selecting a profile and the app starting.">i</i></label><input type="number" id="cfg_boot2" value="500" onchange="saveBootSettings()"></div>
        <div><label>Home Screen Buffer (ms) <i class="tooltip" data-tip="Time needed to buffer the game into memory. Increase on Switch 1, keep as default on Switch 2.">i</i></label><input type="number" id="cfg_boot3" value="2000" onchange="saveBootSettings()"></div>
        
        <div style="grid-column: 1 / -1; margin-top: 10px; border-top: 1px dashed #45475a; padding-top: 15px;">
           <button class="btn btn-danger" style="width: 100%; font-size: 0.9rem;" onclick="rebootHardware()">⚠️ Reboot ESP32 Hardware</button>
        </div>
      </div>
    </details>

    <div class="section">
      <div class="sequence-header">
        <h3 style="margin: 0;">Macro Timeline</h3>
        <div class="profile-controls">
          <input type="text" id="profileName" placeholder="Profile Name...">
          <button class="btn" id="saveBtn" style="width: auto; display: flex; align-items: center;" onclick="saveProfile()">💾 Save & Sync</button>
          <button class="btn btn-danger" style="width: auto;" onclick="deleteProfile()">🗑️ Delete</button>
          
          <select id="profileSelect" onchange="loadProfile()"><option value="">Load Profile...</option></select>
          <button class="btn" style="width: auto; background: #313244; color: #cdd6f4;" onclick="exportProfiles()">📤 Export</button>
          <button class="btn" style="width: auto; background: #313244; color: #cdd6f4;" onclick="document.getElementById('importFile').click()">📥 Import</button>
          <input type="file" id="importFile" style="display:none;" accept=".json" onchange="importProfiles(event)">
        </div>
      </div>
      <div id="timeline"></div>
      <div class="add-btns">
        <button class="add-action-btn" onclick="addStandardAction()">+ ADD ACTION</button>
        <button class="add-phase-btn" onclick="addPhase()">+ ADD CALIBRATED PHASE</button>
      </div>
    </div>

    <div class="status-text" id="gamepadStatus">⌨️ Awaiting Physical Controller...</div>
    <div class="gamepad-container">
      <div class="dpad-container">
        <button class="dbtn d-up" onpointerdown="p('UP')" onpointerup="r('UP')" onpointerleave="r('UP')">▲</button>
        <button class="dbtn d-left" onpointerdown="p('LEFT')" onpointerup="r('LEFT')" onpointerleave="r('LEFT')">◀</button>
        <div class="d-center"></div>
        <button class="dbtn d-right" onpointerdown="p('RIGHT')" onpointerup="r('RIGHT')" onpointerleave="r('RIGHT')">▶</button>
        <button class="dbtn d-down" onpointerdown="p('DOWN')" onpointerup="r('DOWN')" onpointerleave="r('DOWN')">▼</button>
      </div>
      <div class="center-col">
         <button class="plus-btn" onpointerdown="p('PLUS')" onpointerup="r('PLUS')" onpointerleave="r('PLUS')">+</button>
         <button class="home-btn" onpointerdown="p('HOME')" onpointerup="r('HOME')" onpointerleave="r('HOME')">HOME</button>
      </div>
      <div class="action-diamond">
        <button class="abtn btn-x" onpointerdown="p('X')" onpointerup="r('X')" onpointerleave="r('X')">X</button>
        <button class="abtn btn-y" onpointerdown="p('Y')" onpointerup="r('Y')" onpointerleave="r('Y')">Y</button>
        <button class="abtn btn-a" onpointerdown="p('A')" onpointerup="r('A')" onpointerleave="r('A')">A</button>
        <button class="abtn btn-b" onpointerdown="p('B')" onpointerup="r('B')" onpointerleave="r('B')">B</button>
      </div>
    </div>
  </div>

  <script>
    const MS_PER_FRAME = 1000 / 59.7275;
    const BTN_OPTIONS = `<option value="A">A</option><option value="B">B</option><option value="X">X</option><option value="Y">Y</option>
                         <option value="UP">D-UP</option><option value="DOWN">D-DOWN</option><option value="LEFT">D-LEFT</option><option value="RIGHT">D-RIGHT</option>
                         <option value="PLUS">+</option><option value="HOME">HOME</option>`;
    const LOCAL_STORAGE_KEY = 'rng_profiles_stable';
    
    let audioCtx = null; 
    let audioUnlocked = false; 
    let blockIdCounter = 0; 

    let renderInterval = null;
    let isEngineRunning = false;
    let isRestarting = false; 
    let isPolling = false; 
    let isAborting = false; 
    
    let engineStartTime = 0;
    let compiledTimeline = [];
    let currentTotalDur = 0;
    
    let lastBeepTime = 0; 
    let lastBeepSec = -1;
    let hasFinishedRun = false; 
    
    let ws;
    function initWS() {
      ws = new WebSocket(`ws://${window.location.hostname}:81/`);
      ws.onopen = () => { document.getElementById('gamepadStatus').innerHTML = "⚡ Live WebSocket Connected"; };
      ws.onclose = () => { 
        document.getElementById('gamepadStatus').innerHTML = "⚠️ Reconnecting WebSocket..."; 
        setTimeout(initWS, 2000); 
      };
      ws.onerror = (err) => { console.error("WebSocket Error", err); };
    }

    function initAudio() {
      if (audioUnlocked) return;
      if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)();
      if (audioCtx.state === 'suspended') audioCtx.resume();
      
      let osc = audioCtx.createOscillator();
      let gain = audioCtx.createGain();
      gain.gain.value = 0;
      osc.connect(gain);
      gain.connect(audioCtx.destination);
      osc.start(0);
      osc.stop(audioCtx.currentTime + 0.001);
      
      audioUnlocked = true;
      if (isEngineRunning && !isRestarting) { document.getElementById('startBtn').innerText = "RUNNING..."; }
      ['click', 'touchstart', 'keydown'].forEach(e => window.removeEventListener(e, initAudio));
    }
    ['click', 'touchstart', 'keydown'].forEach(e => window.addEventListener(e, initAudio));

    function safeGetProfiles() {
        try {
            return JSON.parse(localStorage.getItem(LOCAL_STORAGE_KEY)) || {};
        } catch(e) {
            localStorage.setItem(LOCAL_STORAGE_KEY, "{}");
            return {};
        }
    }

    function setButtonsState(state) {
      const start = document.getElementById('startBtn');
      const restart = document.getElementById('restartBtn');
      const abort = document.getElementById('stopBtn');
      if (state === "RUNNING") { start.disabled = true; restart.disabled = true; abort.disabled = false; } 
      else if (state === "LOCKED") { start.disabled = true; restart.disabled = true; abort.disabled = true; } 
      else { start.disabled = false; restart.disabled = false; abort.disabled = true; }
    }

    function saveBootSettings() { 
      let s = { 
        b1_en: document.getElementById('cfg_boot1_en').checked,
        b1: document.getElementById('cfg_boot1').value, 
        b2: document.getElementById('cfg_boot2').value, 
        b3: document.getElementById('cfg_boot3').value 
      }; 
      localStorage.setItem('rng_boot_cfg_stable', JSON.stringify(s)); 
    }
    
    function loadBootSettings() { 
      let s;
      try { s = JSON.parse(localStorage.getItem('rng_boot_cfg_stable')); } catch(e) {}
      if (s) { 
        document.getElementById('cfg_boot1_en').checked = s.b1_en !== false;
        document.getElementById('cfg_boot1').value = s.b1 || 1000; 
        document.getElementById('cfg_boot2').value = s.b2 || 500; 
        document.getElementById('cfg_boot3').value = s.b3 || 2000; 
      } 
    }

    function moveUp(id) { const el = document.getElementById(id); if (el.previousElementSibling) el.parentNode.insertBefore(el, el.previousElementSibling); }
    function moveDown(id) { const el = document.getElementById(id); if (el.nextElementSibling) el.parentNode.insertBefore(el.nextElementSibling, el); }
    function renderActionInputs(rowId, type, data = null) {
      const c = document.getElementById(rowId + '_inputs'); c.innerHTML = ''; 
      if (type === 'DELAY') c.innerHTML = `<div><label>Duration (ms)</label><input type="number" class="a-dur" value="${data ? data.dur : 1000}"></div>`;
      else if (type === 'PRESS') { c.innerHTML = `<div><label>Button</label><select class="a-btn">${BTN_OPTIONS}</select></div><div><label>Hold (ms)</label><input type="number" class="a-dur" value="${data ? data.dur : 50}"></div>`; if(data) c.querySelector('.a-btn').value = data.btn; }
      else { c.innerHTML = `<div><label>Button</label><select class="a-btn">${BTN_OPTIONS}</select></div><div><label>Duration (ms)</label><input type="number" class="a-dur" value="${data ? data.dur : 2000}"></div>`; if(data) c.querySelector('.a-btn').value = data.btn; }
    }
    
    function addStandardAction(data = null) {
      const id = 'block_' + blockIdCounter++; const row = document.createElement('div'); row.className = 'action-row block-item'; row.dataset.btype = 'ACTION'; row.id = id;
      const isEn = data ? data.enabled !== false : true;
      row.style.opacity = isEn ? '1' : '0.4';

      const chk = document.createElement('input'); chk.type = 'checkbox'; chk.className = 'block-enable'; chk.checked = isEn; chk.title = 'Mute/Skip'; chk.style.cssText = 'transform: scale(1.2); cursor: pointer; margin-right: 5px;';
      chk.onchange = () => row.style.opacity = chk.checked ? '1' : '0.4';

      const typeSel = document.createElement('select'); typeSel.className = 'a-type'; typeSel.innerHTML = `<option value="DELAY">Delay</option><option value="PRESS">Press</option><option value="HOLD">Hold</option><option value="MASH">Mash</option>`;
      typeSel.onchange = () => renderActionInputs(id, typeSel.value);
      const inps = document.createElement('div'); inps.id = id + '_inputs'; inps.style.display = 'flex'; inps.style.gap = '10px'; inps.style.alignItems = 'center';
      const ctrls = document.createElement('div'); ctrls.className = 'block-controls'; ctrls.innerHTML = `<button class="move-btn" onclick="moveUp('${id}')">↑</button><button class="move-btn" onclick="moveDown('${id}')">↓</button><button class="del-btn" onclick="document.getElementById('${id}').remove()">X</button>`;
      
      row.appendChild(chk); row.appendChild(typeSel); row.appendChild(inps); row.appendChild(ctrls); 
      document.getElementById('timeline').appendChild(row);
      if (data) { typeSel.value = data.type; renderActionInputs(id, data.type, data); } else { renderActionInputs(id, "DELAY"); }
    }
    
    function updateTriggerUI(pid) { const p = document.getElementById(pid); const t = p.querySelector('.p-trig-type').value; document.getElementById(pid + '_trig_inputs').style.display = (t === 'NONE') ? 'none' : 'flex'; }
    function renderTaskInputs(taskId, type, data = null) {
      const c = document.getElementById(taskId + '_inputs'); c.innerHTML = ''; 
      if (type === 'DELAY') c.innerHTML = `<input type="number" class="t-dur" value="${data ? data.dur : 1000}">`;
      else if (type === 'PRESS') { c.innerHTML = `<select class="t-btn">${BTN_OPTIONS}</select><input type="number" class="t-dur" value="${data ? data.dur : 50}">`; if(data) c.querySelector('.t-btn').value = data.btn; }
      else { c.innerHTML = `<select class="t-btn">${BTN_OPTIONS}</select><input type="number" class="t-dur" value="${data ? data.dur : 2000}">`; if(data) c.querySelector('.t-btn').value = data.btn; }
    }
    function addTask(pid, data = null) {
      const id = 'task_' + blockIdCounter++; const row = document.createElement('div'); row.className = 'task-row'; row.id = id;
      const typeSel = document.createElement('select'); typeSel.className = 't-type'; typeSel.style.width = '120px'; typeSel.innerHTML = `<option value="DELAY">Delay</option><option value="PRESS">Press</option><option value="HOLD">Hold</option><option value="MASH">Mash</option>`;
      typeSel.onchange = () => renderTaskInputs(id, typeSel.value);
      const inps = document.createElement('div'); inps.id = id + '_inputs'; inps.style.display = 'flex'; inps.style.gap = '10px';
      const delBtn = document.createElement('button'); delBtn.className = 'del-task-btn'; delBtn.innerText = '×'; delBtn.onclick = () => row.remove();
      row.appendChild(typeSel); row.appendChild(inps); row.appendChild(delBtn); document.getElementById(pid + '_tasks').appendChild(row);
      if (data) { typeSel.value = data.type; renderTaskInputs(id, data.type, data); } else { renderTaskInputs(id, "DELAY"); }
    }
    
    function addPhase(data = null) {
      const id = 'block_' + blockIdCounter++; const phase = document.createElement('div'); phase.className = 'phase-container block-item'; phase.dataset.btype = 'PHASE'; phase.id = id;
      const isEn = data ? data.enabled !== false : true;
      phase.style.opacity = isEn ? '1' : '0.4';
      
      phase.innerHTML = `
        <div class="phase-header">
           <div style="display:flex; align-items:center; gap:8px; width: 100%;">
             <input type="checkbox" class="block-enable" onchange="this.closest('.block-item').style.opacity = this.checked ? '1' : '0.4'" ${isEn ? 'checked' : ''} style="transform: scale(1.2); cursor: pointer;" title="Mute/Skip">
             <span>🟦</span>
             <input type="text" class="p-name-input p-name" placeholder="Phase Name..." value="${data && data.name ? data.name : ''}">
           </div>
           <div class="block-controls"><button class="move-btn" onclick="moveUp('${id}')">↑</button><button class="move-btn" onclick="moveDown('${id}')">↓</button><button class="del-btn" onclick="document.getElementById('${id}').remove()">X</button></div>
        </div>
        <div class="phase-master-timer"><div><label>Unit</label><select class="p-unit"><option value="FRAMES">Frames</option><option value="MS">MS</option></select></div><div><label>Target <i class="tooltip" data-tip="Target duration/frame for this particular phase.">i</i></label><input type="number" class="p-target" value="${data ? data.target : 150}"></div><div><label>Calibration <i class="tooltip" data-tip="Calibration is an offset to Target - it is calculated by entering a value for Hit and pressing 'Update'.">i</i></label><input type="number" class="p-calib" step="0.0001" value="${data ? data.calib : 0}"></div><div><label>Hit</label><input type="number" class="p-hit" placeholder="Hit"></div><button class="calib-btn" onclick="calibratePhase('${id}')">Update</button></div>
        <div class="phase-tasks"><div class="phase-tasks-title"><span>🛠️ Background Setup Tasks <i class="tooltip" data-tip="Actions that run while the master timer is counting down. (e.g., holding A to skip menus).">i</i></span><button class="add-task-btn" onclick="addTask('${id}')">+ Add Task</button></div><div id="${id}_tasks" class="task-list"></div></div>
        <div class="phase-trigger" id="${id}_trigger"><span style="font-size: 0.8rem; color: #bac2de; font-weight: bold; width: 100px; display:flex; align-items:center;">🎯 Final Trigger <i class="tooltip" data-tip="The action executed at the exact microsecond this phase reaches 0.000. Select 'None' to instantly cascade into the next phase.">i</i></span><select class="p-trig-type" onchange="updateTriggerUI('${id}')"><option value="NONE">None (Cascade)</option><option value="PRESS">Press</option><option value="HOLD">Hold</option><option value="MASH">Mash</option></select><div id="${id}_trig_inputs" style="display: flex; gap: 10px;"><select class="p-trig-btn">${BTN_OPTIONS}</select><input type="number" class="p-trig-dur" value="50" style="width: 80px;"></div></div>`;
      document.getElementById('timeline').appendChild(phase);
      if (data) {
        phase.querySelector('.p-unit').value = data.unit; phase.querySelector('.p-trig-type').value = data.triggerType || 'PRESS'; phase.querySelector('.p-trig-btn').value = data.triggerBtn || 'A'; phase.querySelector('.p-trig-dur').value = data.triggerDur || 50;
        if (data.tasks) { data.tasks.forEach(t => addTask(id, t)); }
      } else { phase.querySelector('.p-trig-type').value = 'PRESS'; }
      updateTriggerUI(id); 
    }
    
    function calibratePhase(pid) {
      const p = document.getElementById(pid); const unit = p.querySelector('.p-unit').value; const tgt = parseFloat(p.querySelector('.p-target').value); const cInp = p.querySelector('.p-calib'); const hit = parseFloat(p.querySelector('.p-hit').value);
      let c = parseFloat(cInp.value) || 0; if(!isNaN(tgt) && !isNaN(hit)) { let off = tgt - hit; if (unit === 'FRAMES') off = off * MS_PER_FRAME; cInp.value = parseFloat((c + off).toFixed(4)); p.querySelector('.p-hit').value = ""; }
    }

    async function saveProfile() {
      const name = document.getElementById('profileName').value.trim(); if (!name) return alert("Please enter a profile name");
      const btn = document.getElementById('saveBtn'); btn.innerHTML = "Syncing... <i class='tooltip' data-tip='Saves the profile locally and pushes it to the ESP32 cloud.'>i</i>";
      const sequence = [];
      document.querySelectorAll('.block-item').forEach(b => {
        if (b.dataset.btype === 'ACTION') { 
            sequence.push({ btype: 'ACTION', enabled: b.querySelector('.block-enable').checked, type: b.querySelector('.a-type').value, dur: parseInt(b.querySelector('.a-dur').value) || 50, btn: b.querySelector('.a-btn') ? b.querySelector('.a-btn').value : null }); 
        } else {
          let pData = { btype: 'PHASE', enabled: b.querySelector('.block-enable').checked, name: b.querySelector('.p-name').value, unit: b.querySelector('.p-unit').value, target: parseFloat(b.querySelector('.p-target').value)||0, calib: parseFloat(b.querySelector('.p-calib').value)||0, triggerType: b.querySelector('.p-trig-type').value, triggerBtn: b.querySelector('.p-trig-btn').value, triggerDur: parseInt(b.querySelector('.p-trig-dur').value)||50, tasks: [] };
          b.querySelectorAll('.task-row').forEach(tr => pData.tasks.push({ type: tr.querySelector('.t-type').value, dur: parseInt(tr.querySelector('.t-dur').value)||50, btn: tr.querySelector('.t-btn') ? tr.querySelector('.t-btn').value : null }));
          sequence.push(pData);
        }
      });
      let profiles = safeGetProfiles(); 
      profiles[name] = sequence; 
      profiles._lastProfile = name;
      
      let json = JSON.stringify(profiles); localStorage.setItem(LOCAL_STORAGE_KEY, json); 
      try { await fetch('/save_profiles', { method: 'POST', body: json }); btn.innerHTML = "✅ Saved! <i class='tooltip' data-tip='Saves the profile locally and pushes it to the ESP32 cloud.'>i</i>"; } catch(e) { btn.innerHTML = "⚠️ Local Only <i class='tooltip' data-tip='Saves the profile locally and pushes it to the ESP32 cloud.'>i</i>"; }
      setTimeout(() => btn.innerHTML = "💾 Save & Sync <i class='tooltip' data-tip='Saves the profile locally and pushes it to the ESP32 cloud.'>i</i>", 2000); updateProfileDropdown(); 
    }

    async function deleteProfile() {
      const sel = document.getElementById('profileSelect');
      const name = sel.value;
      if (!name) return alert("Please select a profile to delete first.");
      if (!confirm(`Are you sure you want to permanently delete the profile '${name}'? This will sync to the hardware cloud.`)) return;

      let profiles = safeGetProfiles();
      delete profiles[name];
      if (profiles._lastProfile === name) delete profiles._lastProfile;
      
      let json = JSON.stringify(profiles);
      localStorage.setItem(LOCAL_STORAGE_KEY, json);

      try {
         await fetch('/save_profiles', { method: 'POST', body: json });
         alert(`Profile '${name}' deleted successfully from all devices.`);
      } catch(e) {
         alert(`Profile '${name}' deleted locally, but cloud sync failed.`);
      }

      document.getElementById('timeline').innerHTML = '';
      document.getElementById('profileName').value = '';
      updateProfileDropdown();
    }
    
    function exportProfiles() { const profiles = localStorage.getItem(LOCAL_STORAGE_KEY) || "{}"; const blob = new Blob([profiles], {type: "application/json"}); const url = URL.createObjectURL(blob); const a = document.createElement('a'); a.href = url; a.download = "RNG_Profiles_Backup.json"; document.body.appendChild(a); a.click(); document.body.removeChild(a); URL.revokeObjectURL(url); }
    
    function importProfiles(event) { 
      const file = event.target.files[0]; if (!file) return; 
      const reader = new FileReader(); 
      reader.onload = async (e) => { 
        try { 
          const data = JSON.parse(e.target.result); 
          let existing = safeGetProfiles(); 
          Object.assign(existing, data); 
          let jsonStr = JSON.stringify(existing);
          localStorage.setItem(LOCAL_STORAGE_KEY, jsonStr); 
          
          await fetch('/save_profiles', { method: 'POST', body: jsonStr });
          updateProfileDropdown(); 
          loadProfile(true); 
          alert("Profiles imported successfully and synced to hardware!"); 
        } catch (err) { alert("Error: Invalid JSON file."); } 
      }; 
      reader.readAsText(file); event.target.value = ""; 
    }

    function loadProfile(skipSync = false) { 
      const name = document.getElementById('profileSelect').value; 
      if (!name) return; 
      let profiles = safeGetProfiles(); 
      if (profiles[name]) { 
         document.getElementById('timeline').innerHTML = ''; 
         document.getElementById('profileName').value = name; 
         profiles[name].forEach(d => { if (d.btype === 'ACTION') addStandardAction(d); else addPhase(d); }); 
         
         if (!skipSync && profiles._lastProfile !== name) {
            profiles._lastProfile = name;
            localStorage.setItem(LOCAL_STORAGE_KEY, JSON.stringify(profiles));
            fetch('/save_profiles', { method: 'POST', body: JSON.stringify(profiles) }).catch(e=>{});
         }
      } 
    }
    
    function updateProfileDropdown() { 
      const sel = document.getElementById('profileSelect'); 
      let p = safeGetProfiles(); 
      let currentVal = sel.value;
      sel.innerHTML = '<option value="">Load Profile...</option>'; 
      Object.keys(p).forEach(n => { if(n !== '_lastProfile') sel.innerHTML += `<option value="${n}">${n}</option>`; }); 
      if (currentVal && p[currentVal]) sel.value = currentVal;
    }
    
    window.onload = () => { 
      loadBootSettings();
      initWS(); 
      
      fetch('/get_profiles').then(r=>r.text()).then(t=>{ 
        if(t && t.length >= 2){ 
          try {
             JSON.parse(t); 
             localStorage.setItem(LOCAL_STORAGE_KEY, t); 
          } catch(e) {}
        } 
        updateProfileDropdown(); 
        
        let localData = safeGetProfiles();
        if(localData._lastProfile && localData[localData._lastProfile]) {
            document.getElementById('profileSelect').value = localData._lastProfile;
            loadProfile(true); 
        }
      }).catch(e=>updateProfileDropdown()); 
    };

    function buildExecutionMap(profileName = null) {
       if (profileName && document.getElementById('profileSelect').value !== profileName) {
           document.getElementById('profileSelect').value = profileName; loadProfile(true);
       }

       let map = []; let hwSeq = []; let currentMs = 0; let phaseCount = 1;
       
       const b1_en = document.getElementById('cfg_boot1_en').checked;
       const b1 = parseInt(document.getElementById('cfg_boot1').value) || 1000;
       const b2 = parseInt(document.getElementById('cfg_boot2').value) || 500;
       const b3 = parseInt(document.getElementById('cfg_boot3').value) || 2000;

       let bootDur = 1050; 
       if (b1_en) {
           hwSeq.push(`D:A`, `W:100`, `U:A`, `W:${b1}`);
           hwSeq.push(`D:A`, `W:100`, `U:A`, `W:${b2}`);
           bootDur += 100 + b1 + 100 + b2;
       } else {
           hwSeq.push(`D:A`, `W:100`, `U:A`, `W:${b2}`);
           bootDur += 100 + b2;
       }
       hwSeq.push(`D:HOME`, `W:100`, `U:HOME`, `W:${b3}`);
       hwSeq.push(`D:A`, `W:50`, `U:A`);
       bootDur += 100 + b3 + 50;

       map.push({ start: currentMs, end: currentMs + bootDur, label: "BOOT SEQUENCE", id: null, isPhase: false });
       currentMs += bootDur;

       document.querySelectorAll('.block-item').forEach(b => {
          if (!b.querySelector('.block-enable').checked) return; 

          if (b.dataset.btype === 'ACTION') {
             let dur = parseInt(b.querySelector('.a-dur').value)||50; let type = b.querySelector('.a-type').value; let btn = b.querySelector('.a-btn') ? b.querySelector('.a-btn').value : "";
             if (type==='DELAY') hwSeq.push(`W:${dur}`); else if (type==='PRESS') { hwSeq.push(`P:${btn}:${dur}`); dur+=50; } else if (type==='HOLD') hwSeq.push(`H:${btn}:${dur}`); else hwSeq.push(`M:${btn}:${dur}`);
             map.push({ start: currentMs, end: currentMs + dur, label: `ACTION`, id: b.id, isPhase: false });
             currentMs += dur;
          } else {
             let t = parseFloat(b.querySelector('.p-target').value)||0; let c = parseFloat(b.querySelector('.p-calib').value)||0;
             let dur = (b.querySelector('.p-unit').value === 'FRAMES') ? (t*MS_PER_FRAME)+c : t+c; if (dur<0) dur=0;
             hwSeq.push(`S:${(dur*1000).toFixed(0)}`);
             
             let subTasks = []; let subMs = currentMs;
             let tasksTotalDur = 0;
             
             b.querySelectorAll('.task-row').forEach(tr => {
                 let td = parseInt(tr.querySelector('.t-dur').value)||50; let tt = tr.querySelector('.t-type').value; let tb = tr.querySelector('.t-btn') ? tr.querySelector('.t-btn').value : "";
                if (tt==='DELAY') hwSeq.push(`W:${td}`); else if (tt==='PRESS') { hwSeq.push(`P:${tb}:${td}`); td+=50; } else if (tt==='HOLD') hwSeq.push(`H:${tb}:${td}`); else hwSeq.push(`M:${tb}:${td}`);
                subTasks.push({ start: subMs, end: subMs + td, id: tr.id }); 
                subMs += td;
                tasksTotalDur += td;
             });
             hwSeq.push(`E`);
             
             let customName = b.querySelector('.p-name').value.trim();
             let phaseLabel = customName ? customName : `PHASE ${phaseCount}`;
             
             let actualPhaseDur = Math.max(dur, tasksTotalDur);
             
             map.push({ start: currentMs, end: currentMs + actualPhaseDur, label: phaseLabel, phaseIndex: phaseCount, id: b.id, isPhase: true, subTasks });
             currentMs += actualPhaseDur;
             
             let trT = b.querySelector('.p-trig-type').value; let trD = parseInt(b.querySelector('.p-trig-dur').value)||50; let trB = b.querySelector('.p-trig-btn').value;
             if (trT !== 'NONE') {
                if (trT==='PRESS'){ hwSeq.push(`P:${trB}:${trD}`); trD+=50; } else if (trT==='HOLD'){ hwSeq.push(`H:${trB}:${trD}`); } else { hwSeq.push(`M:${trB}:${trD}`); }
                map.push({ start: currentMs, end: currentMs + trD, label: `TRIGGER`, id: `${b.id}_trigger`, isPhase: false });
                currentMs += trD;
             }
             phaseCount++;
          }
       });
       return { map, hwString: hwSeq.join('|'), totalDur: currentMs, phaseCount: phaseCount - 1 };
    }

    async function triggerRun() {
       if (isRestarting) return;
       let name = document.getElementById('profileName').value.trim() || "Untitled";
       document.getElementById('startBtn').innerText = "COMPILING...";
       setButtonsState("RUNNING");
       let compile = buildExecutionMap(null);
       try {
         let prep = await fetch('/prepare?seq=' + encodeURIComponent(compile.hwString));
         if (prep.ok) {
            await fetch('/execute?p=' + encodeURIComponent(name));
            startRenderLoop(compile, Date.now());
         }
       } catch(e) { stopEngine("COMM ERROR"); }
    }

    function startRenderLoop(compileData, startTime) {
       document.querySelectorAll('.active-block').forEach(el => el.classList.remove('active-block'));
       compiledTimeline = compileData.map;
       currentTotalDur = compileData.totalDur;
       engineStartTime = startTime;
       isEngineRunning = true;
       hasFinishedRun = false; 
       lastBeepSec = -1;
       lastBeepTime = 0; 
       
       setButtonsState("RUNNING");
       document.getElementById('startBtn').innerText = audioUnlocked ? "RUNNING..." : "SYNCED (TAP TO UNMUTE)";
       document.getElementById('totalDisplay').innerText = formatTime(currentTotalDur);
       
       if (renderInterval) clearInterval(renderInterval);
       renderInterval = setInterval(renderFrame, 50); 
    }

    function renderFrame() {
       if (!isEngineRunning || isRestarting) return;
       let elapsed = Date.now() - engineStartTime;
       
       if (elapsed >= currentTotalDur) {
          document.getElementById('timeRemaining').innerText = "0.000";
          
          if (!hasFinishedRun) {
             triggerVisualFlash();
             if (Date.now() - lastBeepTime > 500) { playBeep(800, 'square', 0.3); lastBeepTime = Date.now(); }
             hasFinishedRun = true;
          }
          return; 
       }
       
       let activeItem = compiledTimeline.find(p => elapsed >= p.start && elapsed < p.end);
       
       if (activeItem) {
          let remainingInStage = activeItem.end - elapsed;
          document.getElementById('timeRemaining').innerText = (remainingInStage / 1000).toFixed(3);
          
          document.getElementById('stageLabel').innerText = activeItem.label;
          document.getElementById('phaseDisplay').innerText = activeItem.isPhase ? `${activeItem.phaseIndex} of ${compiledTimeline.filter(p=>p.isPhase).length}` : "Wait";
          
          let nextIdx = compiledTimeline.indexOf(activeItem) + 1;
          document.getElementById('nextDisplay').innerText = compiledTimeline[nextIdx] ? formatTime(compiledTimeline[nextIdx].end - compiledTimeline[nextIdx].start) : "-";

          document.querySelectorAll('.active-block').forEach(el => el.classList.remove('active-block'));
          if (activeItem.id) {
             let dom = document.getElementById(activeItem.id);
             if (dom) {
                 if (activeItem.isPhase) dom.querySelector('.phase-header').classList.add('active-block');
                else dom.classList.add('active-block');
             }
          }
          if (activeItem.subTasks) {
             let activeSub = activeItem.subTasks.find(s => elapsed >= s.start && elapsed < s.end);
             if (activeSub && document.getElementById(activeSub.id)) document.getElementById(activeSub.id).classList.add('active-block');
          }

          if (activeItem.isPhase) {
             let secRem = Math.ceil(remainingInStage / 1000);
             if (secRem <= 5 && secRem !== lastBeepSec) {
                triggerVisualFlash(); playBeep(440, 'sine', 0.1); lastBeepSec = secRem;
             }
          } else {
             lastBeepSec = -1; 
          }
       }
    }

    setInterval(() => {
      if (isRestarting || isPolling) return; 
      isPolling = true;
      
      fetch('/status').then(r => r.text()).then(status => {
        if (status.startsWith("BUSY")) {
           let parts = status.split(":");
           let remoteProfile = parts[1];
           let remoteElapsed = parseInt(parts[2]);
           
           if (!isEngineRunning && remoteProfile !== "TEARDOWN") {
              document.getElementById('startBtn').innerText = "REMOTE SYNC...";
              let compile = buildExecutionMap(remoteProfile);
              startRenderLoop(compile, Date.now() - remoteElapsed);
           } 
           else if (isEngineRunning && remoteProfile !== "TEARDOWN" && !isAborting) {
              if (!hasFinishedRun) {
                  let expectedElapsed = Date.now() - engineStartTime;
                  if (Math.abs(expectedElapsed - remoteElapsed) > 400) {
                      engineStartTime = Date.now() - remoteElapsed; 
                  }
              }
           }
        } else if (status === "IDLE") {
           isAborting = false; 
           if (isEngineRunning) stopEngine("IDLE");
        }
        isPolling = false;
      }).catch(() => { isPolling = false; });
    }, 1000);

    function stopEngine(label) {
       isEngineRunning = false; clearInterval(renderInterval);
       document.querySelectorAll('.active-block').forEach(el => el.classList.remove('active-block'));
       document.getElementById('stageLabel').innerText = label;
       document.getElementById('timeRemaining').innerText = label === "IDLE" ? "0.000" : "---";
       document.getElementById('phaseDisplay').innerText = "-";
       document.getElementById('nextDisplay').innerText = "-";
       document.getElementById('startBtn').innerText = "Start"; 
       setButtonsState("IDLE");
    }

    function stopMacro() { 
       isAborting = true; 
       fetch('/stop').catch(()=>{}); 
       stopEngine("ABORTED"); 
    }
    
    function restartMacro() { 
      if (isEngineRunning || isRestarting) return;
      isRestarting = true;
      setButtonsState("LOCKED");
      document.getElementById('restartBtn').innerText = "CLOSING...";
      document.getElementById('stageLabel').innerText = "TEARDOWN SEQUENCE";
      document.getElementById('timeRemaining').innerText = "---";

      fetch('/close_game').then(() => { 
        setTimeout(() => {
          document.getElementById('restartBtn').innerText = "Restart (R or ☰)";
          isRestarting = false;
          triggerRun();
        }, 3500); 
      }).catch(() => {
        isRestarting = false;
        stopEngine("COMM ERROR");
      });
    }
    
    // V76: Hardware Reboot UI command
    function rebootHardware() {
      if (confirm("Are you sure you want to reboot the ESP32 hardware? This will disconnect the controller temporarily.")) {
        fetch('/reboot').then(() => {
          document.getElementById('stageLabel').innerText = "BOARD REBOOTING...";
          document.getElementById('timeRemaining').innerText = "---";
          setButtonsState("LOCKED");
          setTimeout(() => location.reload(), 4000);
        }).catch(() => { alert("Failed to send reboot command."); });
      }
    }

    const classMap = {
      'UP': '.d-up', 'DOWN': '.d-down', 'LEFT': '.d-left', 'RIGHT': '.d-right',
      'A': '.btn-a', 'B': '.btn-b', 'X': '.btn-x', 'Y': '.btn-y',
      'HOME': '.home-btn', 'PLUS': '.plus-btn'
    };

    const activeBtns = new Set();
    
    function p(btn) { 
      activeBtns.add(btn); 
      let el = document.querySelector(classMap[btn]);
      if (el) el.classList.add('pressed');
      triggerSync(); 
    }
    
    function r(btn) { 
      activeBtns.delete(btn); 
      let el = document.querySelector(classMap[btn]);
      if (el) el.classList.remove('pressed');
      triggerSync(); 
    }
    
    function triggerSync() { 
      if (ws && ws.readyState === WebSocket.OPEN) {
          ws.send(Array.from(activeBtns).join(','));
      } 
    }

    const padMap = { 0: 'B', 1: 'A', 2: 'Y', 3: 'X', 8: 'HOME', 9: 'RESTART', 12: 'UP', 13: 'DOWN', 14: 'LEFT', 15: 'RIGHT' };
    let prevPadState = {}; let activeGamepadIndex = null;
    window.addEventListener("gamepadconnected", (e) => { activeGamepadIndex = e.gamepad.index; document.getElementById('gamepadStatus').innerText = "🎮 Connected"; pollGamepad(); });
    
    function pollGamepad() {
      if (activeGamepadIndex === null) return;
      const pad = navigator.getGamepads()[activeGamepadIndex];
      if (pad) {
        for (let i = 0; i < pad.buttons.length; i++) {
          let isPressed = pad.buttons[i].pressed; let mappedBtn = padMap[i];
          if (mappedBtn) { 
             if (mappedBtn === 'RESTART') { 
                 if (isPressed && !prevPadState[i]) restartMacro(); 
             } else { 
                 if (isPressed && !prevPadState[i]) p(mappedBtn); 
                 else if (!isPressed && prevPadState[i]) r(mappedBtn); 
             } 
          }
          prevPadState[i] = isPressed;
        }
      }
      requestAnimationFrame(pollGamepad);
    }
    
    const keyState = {};
    window.addEventListener('keydown', (e) => {
       if (e.repeat || ['INPUT', 'TEXTAREA', 'SELECT'].includes(document.activeElement.tagName)) return;
       let b = null;
       switch(e.code) {
          case 'ArrowUp': case 'KeyW': b = 'UP'; break;
          case 'ArrowDown': case 'KeyS': b = 'DOWN'; break;
          case 'ArrowLeft': case 'KeyA': b = 'LEFT'; break;
          case 'ArrowRight': case 'KeyD': b = 'RIGHT'; break;
          case 'Enter': b = 'X'; break;
          case 'Space': b = 'A'; break;
          case 'ShiftLeft': case 'ShiftRight': b = 'B'; break;
          case 'KeyR': if (!document.getElementById('restartBtn').disabled) restartMacro(); return;
       }
       if (b) { keyState[e.code] = true; p(b); e.preventDefault(); }
    });
    window.addEventListener('keyup', (e) => {
       let b = null;
       switch(e.code) {
          case 'ArrowUp': case 'KeyW': b = 'UP'; break;
          case 'ArrowDown': case 'KeyS': b = 'DOWN'; break;
          case 'ArrowLeft': case 'KeyA': b = 'LEFT'; break;
          case 'ArrowRight': case 'KeyD': b = 'RIGHT'; break;
          case 'Enter': b = 'X'; break;
          case 'Space': b = 'A'; break;
          case 'ShiftLeft': case 'ShiftRight': b = 'B'; break;
       }
       if (b && keyState[e.code]) { keyState[e.code] = false; r(b); }
    });

    function triggerVisualFlash() { 
      if(!document.getElementById('soundToggle').checked) return; 
      let timerDiv = document.getElementById('timerContainer'); 
      timerDiv.classList.add('flash-active'); 
      setTimeout(() => timerDiv.classList.remove('flash-active'), 120); 
    }
    
    function playBeep(freq, type, duration) { 
      if(!document.getElementById('soundToggle').checked) return; 
      if (!audioCtx) audioCtx = new (window.AudioContext || window.webkitAudioContext)(); 
      if (audioCtx.state === 'suspended') audioCtx.resume();
      
      let osc = audioCtx.createOscillator(); 
      let gainNode = audioCtx.createGain(); 
      osc.type = type; osc.frequency.value = freq; 
      osc.connect(gainNode); gainNode.connect(audioCtx.destination); 
      osc.start(); 
      gainNode.gain.exponentialRampToValueAtTime(0.00001, audioCtx.currentTime + duration); 
      osc.stop(audioCtx.currentTime + duration); 
    }
    function formatTime(ms) { let totalSec = ms / 1000; let m = Math.floor(totalSec / 60); let s = (totalSec % 60).toFixed(3); return m + "m " + s + "s"; }
  </script>
</body>
</html>
)rawliteral";