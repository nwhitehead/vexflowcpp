
// cpp_draw_character(0xe050, 0, 100);

// cpp_draw_line(100, 0, 0, 300)
// for (var angle = 0.0; angle < 2.0 * 3.14159265; angle += 0.01) {
//     cpp_draw_line(100, 100, 100 + Math.cos(angle) * 50, 100 + Math.sin(angle) * 50)
// }

function assert(value) {
    if (!value) {
        throw new Exception("Assertion failed");
    }
}

globalThis.print = cpp_print;

globalThis.console = {
    log(txt) {
        print(txt);
    },
    warn(txt) {
        print(txt);
    }
};

globalThis.document = {
    createElement(t) {
        if (t === 'canvas') {
            return {
                getContext(t) {
                    return {
                        measureText(txt) {
                            //print(`measureText(${txt}) len=${txt.length} font=${this.font}`);
                            let res = cpp_measure_text(txt.codePointAt(0) || 0, this.font);
                            print(JSON.stringify(res));
                            return {
                                width: 20,
                                actualBoundingBoxLeft: 20,
                                actualBoundingBoxRight: 20,
                                fontBoundingBoxAscent: 20,
                                fontBoundingBoxDescent: 20,
                                actualBoundingBoxAscent: 10,
                                actualBoundingBoxDescent: 10,
                            }
                        }
                    };
                }
            };
        }
    }
};

class CanvasContext {
    constructor() {
        // Need canvas field to hold final computed scaled width and height
        this.canvas = { width:0, height: 0 };
        // Current pen position
        this.position = { x: 0, y: 0 };
        // Whether we are drawing a path
        this.inPath = false;
        // Current path
        this.path = [];
    }
    // Wrapped methods
    getTransform() {
        //print(`getTransform`);
        return 1;
    }
    fillText(txt, x, y) {
        //print(`fillText(${txt}, ${x}, ${y}) font=${this.font}`);
        cpp_draw_character(txt.codePointAt(0) || 0, x, y)
        // Render text, txt x y
    }
    beginPath() {
        //print(`beginPath`);
        assert(this.inPath === false);
        this.inPath = true;
        this.path = [];
    }
    bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y) {
        //print(`bezierCurveTo cp1x=${cp1x} cp1y=${cp1y} cp2x=${cp2x} cp2y=${cp2y} x=${x} y=${y}`);
    }
    fill() {
        //print(`fill`);
        assert(this.inPath === true);
        this.inPath = false;
        // Render this.path as fill
        delete this.path;
    }
    fillRect(x, y, width, height) {
        print(`fillRect x=${x} y=${y} width=${width} height=${height}`);
        cpp_fill_rect(x, y, width, height);
        // Render: { type: 'fillRect', x, y, width, height })
    }
    lineTo(x, y) {
        //print(`lineTo x=${x} y=${y}`);
        assert(this.inPath);
        // Render: { type: 'line', start: { ...this.position }, end: { x, y} })
        cpp_draw_line(this.position.x, this.position.y, x, y);
        this.position = { x, y };
    }
    moveTo(x, y) {
        //print(`moveTo x=${x} y=${y}`);
        assert(this.inPath);
        this.position = { x, y };
    }
    restore() {
        //print(`restore`);
    }
    save() {
        //print(`save`);
    }
    scale(x, y) {
        //print(`scale x=${x} y=${y}`);
    }
    stroke() {
        //print(`stroke`);
        assert(this.inPath === true);
        this.inPath = false;
        // Render this.path as stroke
        delete this.path;
    }
}

class Canvas {
    constructor() {
        this.width = 0;
        this.height = 0;
        this.context = new CanvasContext();
    }
    getContext() {
        return this.context;
    }
    // Need to have toDataURL for type detection to pass
    toDataURL() {
        return "<URL>";
    }
}

///////////////////////////////////////////////////

const { Factory, EasyScore, System } = VexFlow;

const canvas = new Canvas();

const vf = new Factory({
    renderer: { elementId: canvas, width: 500, height: 200, backend: 1 },
});

//VexFlow.setMusicFont("Bravura");
//"Petaluma", "Bravura", "Gonville");

const score = vf.EasyScore();
const system = vf.System();

system
    .addStave({
        voices: [
            score.voice(score.notes('C5/q, B4, A4, G#4', { stem: 'up' })),
            score.voice(score.notes('C#4/h, C#4', { stem: 'down' })),        ],
    })
    .addClef('treble')
    .addTimeSignature('4/4');

vf.draw();
