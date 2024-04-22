import { Canvas } from '../src/vexflow_wrap.mjs';
import '../external/vexflow-debug.js.js';

const VF = window.VexFlow;

cpp_register_font('../external/Bravura.otf', 'Bravura');

const width = 1024;
const height = 768

export async function main() {

    const canvas = new Canvas(width, height);
    const ctx = VF.Renderer.buildContext(canvas, 1, width, height);

    // Script does not have lexical scope so can't see the const vf, so expose it globally.
    globalThis.VF = VF;
    globalThis.context = ctx;
    cpp_import_script('../test/test1.js');
}

await main();
