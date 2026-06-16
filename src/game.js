const canvas = document.getElementById('game');
const ctx = canvas.getContext('2d');

const keys = new Set();
const action = { pressed: false, last: false };
window.addEventListener('keydown', (event) => {
  keys.add(event.key.toLowerCase());
  if (event.key.toLowerCase() === 'e') action.pressed = true;
});
window.addEventListener('keyup', (event) => keys.delete(event.key.toLowerCase()));

const world = { width: 2400, height: 1500 };
const depots = [
  { name: 'Stadtlager', x: 360, y: 340, color: '#f5c542' },
  { name: 'Möbelhof', x: 1960, y: 420, color: '#66d9ef' },
  { name: 'Baumarkt', x: 1820, y: 1180, color: '#a6e22e' },
  { name: 'Hafen', x: 520, y: 1180, color: '#fd971f' }
];

const player = { mode: 'car', x: 620, y: 760, angle: -0.1, speed: 0, footX: 0, footY: 0 };
const progress = { level: 1, xp: 0, completedJobs: 0, deliveredPallets: 0, message: 'Fahre oder laufe zum gelben Depot und drücke E.' };
let trailer = { axles: 2, cargo: 0 };
let job = null;
const logoImage = new Image();
logoImage.src = 'assets/logo.svg';

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
  const trailerDrag = trailer.axles === 3 ? 0.9 : 1;
  player.speed += (throttle * 230 - brake * 260) * dt * trailerDrag;
  player.speed *= 0.985;
  player.speed = clamp(player.speed, -130, 260);
  player.angle += clamp(steer, -1, 1) * player.speed * 0.0025 * dt * 60;
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
  drawBrandMark(-45, 0, 28, '#e9f1f7');
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
  ctx.fillStyle = '#111';
  const axleXs = trailer.axles === 3 ? [-35, 0, 35] : [-25, 25];
  axleXs.forEach((x) => { ctx.fillRect(x, -42, 18, 10); ctx.fillRect(x, 32, 18, 10); });
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
  ctx.fillRect(18, 18, 420, 165);
  ctx.fillStyle = '#eef4f8';
  ctx.font = '20px sans-serif';
  ctx.fillText(`Level ${progress.level}  XP ${Math.round(progress.xp * 100)}%`, 38, 52);
  ctx.fillText(`Modus: ${player.mode === 'car' ? 'Auto' : 'zu Fuß'} | Anhänger: ${trailer.axles} Achsen`, 38, 82);
  ctx.fillText(`Jobs: ${progress.completedJobs} | Paletten: ${progress.deliveredPallets}`, 38, 112);
  ctx.fillStyle = '#b7e4ff';
  ctx.fillText(job ? `Auftrag: ${job.from.name} → ${job.to.name}` : 'Kein Auftrag aktiv', 38, 142);
  ctx.fillStyle = '#ffe08a';
  ctx.fillText(progress.message.slice(0, 46), 38, 172);
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
