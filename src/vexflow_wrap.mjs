
// cpp_draw_character(0xe050, 0, 100);

// cpp_draw_line(100, 0, 0, 300)
// for (var angle = 0.0; angle < 2.0 * 3.14159265; angle += 0.01) {
//     cpp_draw_line(100, 100, 100 + Math.cos(angle) * 50, 100 + Math.sin(angle) * 50)
// }

cpp_print('hi');

const globalFontScale = 5.5;


globalThis.print = cpp_print;

globalThis.assert = function(condition) {
    if (!condition) {
        throw "Assertion failed";
    }
}

globalThis.console = {
    log(txt) {
        print(txt);
    },
    warn(txt) {
        print(txt);
    },
    error(txt) {
        print(txt);
    }
};

globalThis.log = {
    debug(msg) {
        print(msg);
    }
};

globalThis.window = {
    addEventListener() {},
    setTimeout() {},
};

function parseFont(fontname) {
    if (fontname === undefined) {
        return null;
    }
    const parts = fontname.split(',');
    const part = parts[0];
    const match = part.match(/^(\d+)pt (.*)/);
    if (match) {
        return {
            font: match[2],
            size: Number(match[1]),
        }
    }
    return null;
}

globalThis.document = {
    getElementById(id) {
        print(`getElementById(${id})`);
        return {}
    },
    createElement(t) {
        if (t === 'canvas') {
            return {
                getContext(t) {
                    return {
                        measureText(txt) {
                            const { font, size } = parseFont(this.font);
                            const scale = cpp_get_font_scale(font, size) * globalFontScale;
                            print(`measureText(${txt}) fullFont=${this.font} font=${font} size=${size} scale=${scale}`);
                            let res = cpp_measure_text(txt.codePointAt(0) || 0, font, scale);
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
        this.fontScale = 0.03;
    }
    // Wrapped methods
    getTransform() {
        //print(`getTransform`);
        return 1;
    }
    fillText(txt, x, y) {
        const { font, size } = parseFont(this.font);
        const scale = cpp_get_font_scale(font, size) * globalFontScale;
        print(`fillText(${txt}, ${x}, ${y}) font=${this.font}`);
        cpp_draw_character(txt.codePointAt(0) || 0, x, y, font, scale)
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

export class Canvas {
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

