var dungeon = new Dungeon(79, 39, makeParams());
var canvas = document.getElementById('c');
dungeon.draw(canvas);

function clicked(id, func) {
  document.getElementById(id).addEventListener('click', func);
}

var i = false;
function runUntilDone(interval, func, mode='anim') {
  if (mode == 'anim') {
    if (i) return;
    i = setInterval(function() {
      if (!func.bind(dungeon)()) {
        clearInterval(i);
        i = false;
      }
      dungeon.draw(canvas);
    }, interval);
  } else if (mode == 'step') {
    func.bind(dungeon)();
    dungeon.draw(canvas);
  } else if (mode == 'fast') {
    while (func.bind(dungeon)()) {};
    dungeon.draw(canvas);
  }
}

function makeParams() {
  var params = {};
  var keys = [ 'room_density', 'straightness', 'extra_doors' ];

  for (var i = 0; i < keys.length; ++i) {
    var p = keys[i];
    params[p] = document.getElementById(p).value / 100;
  }

  return params;
}

var stage = 0;
function reset() {
  dungeon.reset(makeParams());
  dungeon.reveal();
  dungeon.draw(canvas);
  stage = 0;
}

clicked('reset', reset);

clicked('step', function(e) {
  switch (stage) {
    case 0:
      reset();
      while (dungeon.placeRoom()) {};
      while (dungeon.step()) {};
      break;

    case 1:
      if (dungeon.connectRegions()) --stage;
      break;

    case 2:
      while (dungeon.cleanDeadEnds()) {};
      break;

    case 3:
      if (dungeon.connectSections()) --stage;
      break;
  }

  ++stage;
  dungeon.draw(canvas);
});

clicked('all', function(e) {
  dungeon.generateAll(makeParams());
  dungeon.reveal();
  dungeon.draw(canvas);
  stage = 9;
});

reset();
