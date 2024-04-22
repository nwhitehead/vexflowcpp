
// Scale so that 30pt Bravura maps to right size.
// Not sure why this is needed.
const globalFontScale = 5.5;

globalThis.assert = function(condition) {
    if (!condition) {
        throw "Assertion failed";
    }
}

globalThis.print = cpp_print;

globalThis.console = {
    log(txt) {
        cpp_print(txt);
    },
    warn(txt) {
        cpp_print(txt);
    },
    error(txt) {
        cpp_print(txt);
    }
};

// Need to have window object so that we get window.VexFlow
// Should not need any methods (setTimeout etc.)
globalThis.window = {};

/// Parse full fontname like "30pt Bravur,Academico" into { font: 'Bravura', size: 30 }
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
        // Should never get here
        assert(false);
    },
    createElement(t) {
        assert(t === 'canvas');
        // Canvases created during rendering are for font measuring only.
        return {
            getContext(t) {
                return {
                    measureText(txt) {
                        const { font, size } = parseFont(this.font);
                        const scale = cpp_get_font_scale(font, size) * globalFontScale;
                        let res = cpp_measure_text(txt.codePointAt(0) || 0, font, scale);
                        return res;
                    }
                };
            }
        };
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
        const { font, size } = parseFont(this.font);
        const scale = cpp_get_font_scale(font, size) * globalFontScale;
        //print(`fillText(${txt}, ${x}, ${y}) font=${this.font}`);
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
        //print(`fillRect x=${x} y=${y} width=${width} height=${height}`);
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

