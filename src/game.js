const canvas = document.getElementById('game');
const ctx = canvas.getContext('2d');
const companyForm = document.getElementById('companyForm');
const companyNameInput = document.getElementById('companyName');

const keys = new Set();
let gameStarted = false;
let companyName = 'Meine Transportfirma';
const action = { pressed: false, last: false };
const shift = { up: false, down: false, lastUp: false, lastDown: false };
window.addEventListener('keydown', (event) => {
  if (event.target instanceof HTMLInputElement) return;
  keys.add(event.key.toLowerCase());
  if (event.key.toLowerCase() === 'e') action.pressed = true;
  if (event.key.toLowerCase() === 'c') toggleCameraView();
  if (event.key.toLowerCase() === 'r') shift.up = true;
  if (event.key.toLowerCase() === 'q') shift.down = true;
});
window.addEventListener('keyup', (event) => keys.delete(event.key.toLowerCase()));

const world = { width: 2400, height: 1500 };
const depots = [
  { name: 'Stadtlager', x: 360, y: 340, color: '#f5c542' },
  { name: 'Möbelhof', x: 1960, y: 420, color: '#66d9ef' },
  { name: 'Baumarkt', x: 1820, y: 1180, color: '#a6e22e' },
  { name: 'Hafen', x: 520, y: 1180, color: '#fd971f' }
];

const player = { mode: 'car', view: 'cockpit', x: 620, y: 760, angle: -0.1, speed: 0, footX: 0, footY: 0 };
const progress = { level: 1, xp: 0, completedJobs: 0, deliveredPallets: 0, message: 'Fahre oder laufe zum gelben Depot und drücke E.' };
let trailer = { axles: 2, cargo: 0 };
const vehicle = {
  brand: 'ORION',
  model: 'Vektor 240D',
  mass: 1680,
  drag: 0.34,
  rollingResistance: 13.5,
  wheelBase: 2.72,
  steerAngle: 0,
  rpm: 850,
  gear: 1,
  clutch: 1,
  fuel: 68,
  brakeTemp: 42,
  traction: 1,
  absPulse: 0,
  odometer: 184235.4
};
const gearRatios = [-3.42, 0, 3.8, 2.15, 1.36, 1.0, 0.82];
const finalDrive = 3.73;
const brandPalette = { deep: '#0c1824', metal: '#d7e2ea', accent: '#ffcc45', glass: '#7fb2d7' };
let job = null;
const logoImage = new Image();
logoImage.src = 'assets/logo.svg';

companyForm.addEventListener('submit', (event) => {
  event.preventDefault();
  companyName = sanitizeCompanyName(companyNameInput.value);
  companyNameInput.value = companyName;
  companyForm.classList.add('is-hidden');
  gameStarted = true;
  progress.message = `Willkommen bei ${companyName}! Nimm den ersten Auftrag am Depot an.`;
});

function sanitizeCompanyName(value) {
  const cleaned = value.replace(/[^\p{L}\p{N} .&-]/gu, '').replace(/\s+/g, ' ').trim();
  return cleaned.slice(0, 22) || 'Meine Transportfirma';
}

function has(k) { return keys.has(k); }
function clamp(v, min, max) { return Math.max(min, Math.min(max, v)); }
function dist(a, b, x, y) { return Math.hypot(a - x, b - y); }
function actionOnce() {
  const now = action.pressed || gamepadAction();
  const once = now && !action.last;
  action.last = now;
  action.pressed = false;
  return once;
}

function gamepadState() {
  const pads = navigator.getGamepads ? [...navigator.getGamepads()].filter(Boolean) : [];
  const pad = pads[0];
  if (!pad) return { steer: 0, throttle: 0, brake: 0, action: false };
  const steer = Math.abs(pad.axes[0] || 0) > 0.08 ? pad.axes[0] : 0;
  const triggerGas = pad.buttons[7]?.value || 0;
  const triggerBrake = pad.buttons[6]?.value || 0;
  const axisDrive = pad.axes[1] || 0;
  return {
    steer,
    throttle: Math.max(triggerGas, axisDrive < -0.2 ? -axisDrive : 0),
    brake: Math.max(triggerBrake, axisDrive > 0.2 ? axisDrive : 0),
    action: Boolean(pad.buttons[0]?.pressed)
  };
}
function gamepadAction() { return gamepadState().action; }

function toggleCameraView() {
  if (!gameStarted || player.mode !== 'car') return;
  player.view = player.view === 'cockpit' ? 'chase' : 'cockpit';
  progress.message = player.view === 'cockpit'
    ? 'Cockpitkamera: Innenraum, Lenkradlogo und Motorhauben-Emblem aktiv.'
    : 'Außenkamera aktiv.';
}

function currentDepot(radius = 95) {
  const x = player.mode === 'car' ? player.x : player.footX;
  const y = player.mode === 'car' ? player.y : player.footY;
  return depots.find((depot) => dist(x, y, depot.x, depot.y) < radius);
}

function addXp(amount) {
  progress.xp += amount;
  while (progress.xp >= 1) {
    progress.xp -= 1;
    progress.level += 1;
    progress.message = `Level ${progress.level} erreicht!`;
  }
}

function createJob(from) {
  const targets = depots.filter((depot) => depot !== from);
  const to = targets[Math.floor(Math.random() * targets.length)];
  return { from, to, pallets: 5, loaded: false };
}

function update(dt) {
  if (!gameStarted) return;
  const pad = gamepadState();
  if (keys.has('1')) trailer.axles = 2;
  if (keys.has('2')) {
    if (progress.level >= 10) trailer.axles = 3;
    else progress.message = '3-Achs-Anhänger wird ab Level 10 freigeschaltet.';
  }

  if (player.mode === 'car') updateCar(dt, pad);
  else updateFoot(dt, pad);

  if (actionOnce()) interact();
}

function updateCar(dt, pad) {
  const throttle = (has('w') || has('arrowup') ? 1 : 0) + pad.throttle;
  const brake = (has('s') || has('arrowdown') ? 1 : 0) + pad.brake;
  const steer = (has('a') || has('arrowleft') ? -1 : 0) + (has('d') || has('arrowright') ? 1 : 0) + pad.steer;
  const steerInput = clamp(steer, -1, 1);
  const trailerMass = trailer.axles === 3 ? 820 : 540;
  const cargoMass = trailer.cargo * 85;
  const totalMass = vehicle.mass + trailerMass + cargoMass;
  vehicle.steerAngle += (steerInput - vehicle.steerAngle) * Math.min(1, dt * 7.5);
  const speedKmh = Math.abs(player.speed) * 0.34;
  const upshift = shift.up && !shift.lastUp;
  const downshift = shift.down && !shift.lastDown;
  shift.lastUp = shift.up;
  shift.lastDown = shift.down;
  shift.up = false;
  shift.down = false;
  if (speedKmh > 4 && (upshift || downshift)) vehicle.gear += upshift ? 1 : -1;
  vehicle.gear = clamp(vehicle.gear, player.speed < -4 ? 0 : 1, gearRatios.length - 2);
  const ratio = gearRatios[vehicle.gear + 1] || gearRatios[2];
  const engineTorque = throttle * (430 - Math.max(0, vehicle.rpm - 2600) * 0.055);
  const driveForce = engineTorque * ratio * finalDrive * 0.82 / 0.34;
  const brakeForce = brake * (has(' ') ? 9800 : 6200);
  const airDrag = vehicle.drag * player.speed * Math.abs(player.speed) * 0.42;
  const rolling = vehicle.rollingResistance * totalMass * Math.sign(player.speed || 1) * 0.045;
  const netForce = driveForce - brakeForce * Math.sign(player.speed || 1) - airDrag - rolling;
  player.speed += (netForce / totalMass) * dt * 48;
  player.speed *= 1 - dt * (0.006 + cargoMass / 380000);
  player.speed = clamp(player.speed, -95, 305);
  vehicle.traction = clamp(1 - Math.abs(vehicle.steerAngle) * speedKmh / 165, 0.62, 1);
  vehicle.absPulse = brake > 0.75 && speedKmh > 35 ? Math.sin(performance.now() * 0.04) * 0.5 + 0.5 : 0;
  vehicle.rpm += (((Math.abs(player.speed) * Math.abs(ratio) * finalDrive * 18) + 780 + throttle * 950) - vehicle.rpm) * Math.min(1, dt * 5);
  vehicle.rpm = clamp(vehicle.rpm, 760, 4200);
  vehicle.brakeTemp = clamp(vehicle.brakeTemp + brake * speedKmh * dt * 0.7 - dt * 6, 35, 610);
  vehicle.fuel = clamp(vehicle.fuel - throttle * (0.0009 + speedKmh * 0.000003) * dt, 0, 68);
  vehicle.odometer += Math.abs(player.speed) * dt / 3600;
  player.angle += vehicle.steerAngle * player.speed * 0.0022 * dt * 60 * vehicle.traction;
  player.x += Math.cos(player.angle) * player.speed * dt;
  player.y += Math.sin(player.angle) * player.speed * dt;
  player.x = clamp(player.x, 80, world.width - 80);
  player.y = clamp(player.y, 80, world.height - 80);
}

function updateFoot(dt, pad) {
  const dx = (has('a') || has('arrowleft') ? -1 : 0) + (has('d') || has('arrowright') ? 1 : 0) + pad.steer;
  const dy = (has('w') || has('arrowup') ? -1 : 0) + (has('s') || has('arrowdown') ? 1 : 0);
  const len = Math.hypot(dx, dy) || 1;
  player.footX = clamp(player.footX + (dx / len) * 170 * dt, 40, world.width - 40);
  player.footY = clamp(player.footY + (dy / len) * 170 * dt, 40, world.height - 40);
}

function interact() {
  if (player.mode === 'car' && Math.abs(player.speed) < 18) {
    player.mode = 'foot';
    player.footX = player.x - Math.sin(player.angle) * 48;
    player.footY = player.y + Math.cos(player.angle) * 48;
    progress.message = 'Du bist ausgestiegen. Gehe zur Tür und drücke E zum Einsteigen.';
    return;
  }

  if (player.mode === 'foot' && dist(player.footX, player.footY, player.x, player.y) < 80) {
    player.mode = 'car';
    progress.message = 'Wieder im Auto.';
    return;
  }

  const depot = currentDepot();
  if (!depot) return;
  if (!job) {
    job = createJob(depot);
    trailer.cargo = job.pallets;
    job.loaded = true;
    progress.message = `${job.pallets} Paletten geladen: ${job.from.name} → ${job.to.name}.`;
  } else if (job.to === depot && job.loaded) {
    trailer.cargo = 0;
    progress.completedJobs += 1;
    progress.deliveredPallets += job.pallets;
    addXp(0.1); // 2-3 five-pallet jobs are roughly one quarter level.
    progress.message = `Auftrag abgeschlossen: +10% Level. ${progress.completedJobs} Jobs erledigt.`;
    job = null;
  } else {
    progress.message = `Ziel ist ${job.to.name}.`;
  }
}

function camera() {
  const x = player.mode === 'car' ? player.x : player.footX;
  const y = player.mode === 'car' ? player.y : player.footY;
  if (player.mode === 'car' && player.view === 'cockpit') {
    const lookAhead = 260;
    return {
      x: clamp(x + Math.cos(player.angle) * lookAhead - canvas.width / 2, 0, world.width - canvas.width),
      y: clamp(y + Math.sin(player.angle) * lookAhead - canvas.height / 2, 0, world.height - canvas.height)
    };
  }
  return { x: clamp(x - canvas.width / 2, 0, world.width - canvas.width), y: clamp(y - canvas.height / 2, 0, world.height - canvas.height) };
}

function draw() {
  const cam = camera();
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.save();
  ctx.translate(-cam.x, -cam.y);
  drawWorld();
  depots.forEach(drawDepot);
  drawTrailer();
  drawCar();
  if (player.mode === 'foot') drawPerson();
  ctx.restore();
  if (player.mode === 'car' && player.view === 'cockpit') drawCockpitView();
  drawHud();
}

function drawWorld() {
  ctx.fillStyle = '#27402d';
  ctx.fillRect(0, 0, world.width, world.height);
  ctx.strokeStyle = '#59636b';
  ctx.lineWidth = 90;
  ctx.lineCap = 'round';
  ctx.beginPath();
  ctx.moveTo(250, 760); ctx.lineTo(2140, 760); ctx.lineTo(1980, 1160); ctx.lineTo(520, 1180); ctx.lineTo(360, 360); ctx.lineTo(1960, 420);
  ctx.stroke();
  ctx.strokeStyle = '#d6d6c2';
  ctx.setLineDash([28, 36]);
  ctx.lineWidth = 5;
  ctx.stroke();
  ctx.setLineDash([]);
}

function drawDepot(depot) {
  ctx.fillStyle = depot.color;
  ctx.fillRect(depot.x - 70, depot.y - 45, 140, 90);
  ctx.fillStyle = '#182026';
  ctx.font = '22px sans-serif';
  ctx.fillText(depot.name, depot.x - 65, depot.y - 58);
  ctx.fillStyle = '#8b5a2b';
  for (let i = 0; i < 5; i++) ctx.fillRect(depot.x - 55 + i * 24, depot.y + 12, 18, 18);
}

function drawCar() {
  ctx.save();
  ctx.translate(player.x, player.y);
  ctx.rotate(player.angle);
  ctx.fillStyle = '#8aa4b8';
  ctx.fillRect(-58, -28, 116, 56);
  ctx.fillStyle = '#557080';
  ctx.fillRect(-20, -25, 44, 50);
  ctx.fillStyle = '#101820';
  ctx.fillRect(30, -31, 18, 8); ctx.fillRect(30, 23, 18, 8);
  // Prominentes Motorhauben-Emblem der fiktiven Marke, analog zu realen Pkw.
  drawBrandMark(38, 0, 30, '#e9f1f7');
  ctx.fillStyle = '#edf5fb';
  ctx.font = 'bold 11px sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText(vehicle.brand, -18, 4);
  ctx.restore();
}


function drawBrandMark(x, y, size, background = '#ffffff') {
  ctx.save();
  ctx.translate(x, y);
  ctx.fillStyle = background;
  ctx.beginPath();
  ctx.arc(0, 0, size * 0.56, 0, Math.PI * 2);
  ctx.fill();
  if (logoImage.complete && logoImage.naturalWidth > 0) {
    ctx.drawImage(logoImage, -size / 2, -size / 2, size, size);
  } else {
    ctx.strokeStyle = '#050505';
    ctx.lineWidth = Math.max(4, size * 0.12);
    ctx.lineCap = 'round';
    ctx.beginPath();
    ctx.arc(0, 0, size * 0.36, Math.PI * 0.18, Math.PI * 1.72);
    ctx.stroke();
    ctx.beginPath();
    ctx.arc(size * 0.08, size * 0.04, size * 0.28, Math.PI * 0.2, Math.PI * 0.82);
    ctx.stroke();
  }
  ctx.restore();
}

function drawCockpitView() {
  const horizon = 260;
  const gradient = ctx.createLinearGradient(0, 0, 0, canvas.height);
  gradient.addColorStop(0, 'rgba(4, 10, 16, .1)');
  gradient.addColorStop(.42, 'rgba(8, 16, 24, .22)');
  gradient.addColorStop(1, 'rgba(2, 5, 8, .94)');
  ctx.fillStyle = gradient;
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // Motorhaube mit originalgetreu wiederholtem Marken-Emblem im Cockpitblick.
  ctx.fillStyle = '#8aa4b8';
  ctx.beginPath();
  ctx.moveTo(300, 430); ctx.quadraticCurveTo(640, 350, 980, 430);
  ctx.lineTo(1100, 570); ctx.quadraticCurveTo(640, 620, 180, 570); ctx.closePath();
  ctx.fill();
  ctx.strokeStyle = 'rgba(255,255,255,.25)';
  ctx.lineWidth = 3;
  ctx.stroke();
  drawBrandMark(640, 455, 54, '#f2f6f9');

  // Dashboard, A-Säulen und Windschutzscheibe.
  ctx.fillStyle = '#111922';
  ctx.beginPath(); ctx.moveTo(0, 585); ctx.lineTo(1280, 585); ctx.lineTo(1280, 720); ctx.lineTo(0, 720); ctx.closePath(); ctx.fill();
  ctx.fillStyle = 'rgba(7, 12, 18, .92)';
  ctx.beginPath(); ctx.moveTo(0, 0); ctx.lineTo(130, 0); ctx.lineTo(250, 720); ctx.lineTo(0, 720); ctx.closePath(); ctx.fill();
  ctx.beginPath(); ctx.moveTo(1280, 0); ctx.lineTo(1150, 0); ctx.lineTo(1030, 720); ctx.lineTo(1280, 720); ctx.closePath(); ctx.fill();
  ctx.fillStyle = 'rgba(130, 178, 215, .13)';
  ctx.fillRect(150, 40, 980, horizon);

  drawInstrumentCluster();
  drawSteeringWheel();
  drawCenterConsole();
}

function drawSteeringWheel() {
  ctx.save();
  ctx.translate(640, 610);
  ctx.rotate(vehicle.steerAngle * 0.9);
  ctx.strokeStyle = '#07090c';
  ctx.lineWidth = 34;
  ctx.beginPath(); ctx.arc(0, 0, 118, 0, Math.PI * 2); ctx.stroke();
  ctx.strokeStyle = '#28313b'; ctx.lineWidth = 18; ctx.beginPath(); ctx.arc(0, 0, 106, 0, Math.PI * 2); ctx.stroke();
  ctx.fillStyle = '#151d26';
  ctx.beginPath(); ctx.roundRect(-76, -40, 152, 80, 28); ctx.fill();
  ctx.strokeStyle = '#0a0d11'; ctx.lineWidth = 18;
  [[-82,-12,-26,-5],[82,-12,26,-5],[0,86,0,36]].forEach(([x1,y1,x2,y2]) => { ctx.beginPath(); ctx.moveTo(x1,y1); ctx.lineTo(x2,y2); ctx.stroke(); });
  // Lenkrad-Branding prominent im Fahrerzentrum.
  drawBrandMark(0, 0, 58, brandPalette.metal);
  ctx.restore();
}

function drawInstrumentCluster() {
  const speedKmh = Math.round(Math.abs(player.speed) * 0.34);
  ctx.fillStyle = '#05080c';
  ctx.beginPath(); ctx.roundRect(405, 505, 470, 128, 28); ctx.fill();
  drawGauge(500, 568, 54, speedKmh, 260, 'km/h');
  drawGauge(780, 568, 54, Math.round(vehicle.rpm / 100), 42, 'x100 rpm');
  ctx.fillStyle = vehicle.absPulse > .2 ? '#ffd15c' : '#87d37c';
  ctx.font = 'bold 18px sans-serif';
  ctx.fillText(`Gang ${vehicle.gear === 0 ? 'R' : vehicle.gear}`, 610, 545);
  ctx.fillText(`Diesel ${vehicle.fuel.toFixed(1)} l`, 590, 575);
  ctx.fillText(`Bremse ${Math.round(vehicle.brakeTemp)}°C`, 585, 605);
}

function drawGauge(x, y, r, value, max, label) {
  ctx.save(); ctx.translate(x, y);
  ctx.strokeStyle = '#2e4254'; ctx.lineWidth = 10; ctx.beginPath(); ctx.arc(0,0,r,Math.PI*.8,Math.PI*2.2); ctx.stroke();
  ctx.strokeStyle = '#ffcc45'; ctx.beginPath(); ctx.arc(0,0,r,Math.PI*.8,Math.PI*.8+(Math.PI*1.4)*clamp(value/max,0,1)); ctx.stroke();
  ctx.fillStyle = '#eef4f8'; ctx.font = 'bold 20px sans-serif'; ctx.textAlign = 'center'; ctx.fillText(value,0,4);
  ctx.font = '11px sans-serif'; ctx.fillText(label,0,25); ctx.restore();
}

function drawCenterConsole() {
  ctx.fillStyle = '#0b1118'; ctx.beginPath(); ctx.roundRect(900, 520, 165, 145, 18); ctx.fill();
  ctx.fillStyle = '#9fe0ff'; ctx.font = 'bold 15px sans-serif';
  ctx.fillText(vehicle.model, 922, 552);
  ctx.fillText(`${vehicle.odometer.toFixed(1)} km`, 922, 580);
  ctx.fillStyle = '#dce8ee'; ctx.fillText('ABS  ESP  RET', 922, 613);
}

function drawTrailer() {
  const backX = player.x - Math.cos(player.angle) * 120;
  const backY = player.y - Math.sin(player.angle) * 120;
  ctx.save();
  ctx.translate(backX, backY);
  ctx.rotate(player.angle);
  ctx.fillStyle = trailer.axles === 3 ? '#67727e' : '#735f4a';
  ctx.fillRect(-70, -32, 120, 64);
  ctx.fillStyle = '#b78c52';
  for (let i = 0; i < trailer.cargo; i++) ctx.fillRect(-58 + i * 22, -18, 16, 36);
  drawBrandMark(30, 0, 30, '#f3ead7');
  drawTrailerInscription();
  ctx.fillStyle = '#111';
  const axleXs = trailer.axles === 3 ? [-35, 0, 35] : [-25, 25];
  axleXs.forEach((x) => { ctx.fillRect(x, -42, 18, 10); ctx.fillRect(x, 32, 18, 10); });
  ctx.restore();
}


function drawTrailerInscription() {
  const label = companyName.toUpperCase();
  ctx.save();
  ctx.textAlign = 'center';
  ctx.textBaseline = 'middle';
  ctx.font = 'bold 13px sans-serif';
  ctx.lineWidth = 4;
  ctx.strokeStyle = 'rgba(12, 18, 24, .82)';
  ctx.fillStyle = '#fff6d5';
  ctx.strokeText(label, -18, -2, 78);
  ctx.fillText(label, -18, -2, 78);
  ctx.restore();
}

function drawPerson() {
  ctx.fillStyle = '#f7d7b6';
  ctx.beginPath();
  ctx.arc(player.footX, player.footY - 12, 9, 0, Math.PI * 2);
  ctx.fill();
  ctx.fillStyle = '#2f7dd1';
  ctx.fillRect(player.footX - 7, player.footY - 4, 14, 24);
}

function drawHud() {
  ctx.fillStyle = 'rgba(8, 12, 18, .78)';
  ctx.fillRect(18, 18, 440, 195);
  ctx.fillStyle = '#eef4f8';
  ctx.font = '20px sans-serif';
  ctx.fillText(`Level ${progress.level}  XP ${Math.round(progress.xp * 100)}%`, 38, 52);
  ctx.fillText(`Modus: ${player.mode === 'car' ? `Auto/${player.view}` : 'zu Fuß'} | Anhänger: ${trailer.axles} Achsen`, 38, 82);
  ctx.fillText(`Firma: ${companyName.slice(0, 24)}`, 38, 112);
  ctx.fillText(`Jobs: ${progress.completedJobs} | Paletten: ${progress.deliveredPallets}`, 38, 142);
  ctx.fillStyle = '#b7e4ff';
  ctx.fillText(job ? `Auftrag: ${job.from.name} → ${job.to.name}` : 'Kein Auftrag aktiv', 38, 172);
  ctx.fillStyle = '#ffe08a';
  ctx.fillText(progress.message.slice(0, 46), 38, 202);
}

let last = performance.now();
function loop(now) {
  const dt = Math.min((now - last) / 1000, 0.05);
  last = now;
  update(dt);
  draw();
  requestAnimationFrame(loop);
}
requestAnimationFrame(loop);
