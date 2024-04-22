import { Canvas } from '../src/vexflow_wrap.mjs';

cpp_register_font('../external/Bravura.otf', 'Bravura');

export async function main() {

    cpp_import_script('../external/vexflow-debug.js.js');

    const { Factory, EasyScore, System } = window.VexFlow;

    const canvas = new Canvas();

    const vf = new Factory({
        renderer: { elementId: canvas, width: 1024, height: 800, backend: 1 },
    });

    const score = vf.EasyScore();
    const system = vf.System();
    
    system.addStave({
        voices: [
            score.voice(score.notes('C#5/q, B4, A4, G#4', {stem: 'up'})),
            score.voice(score.notes('C#4/h, C#4', {stem: 'down'}))
        ],
    }).addClef('treble').addTimeSignature('4/4');

    system.addStave({
        voices: [
            score.voice(score.notes('C#3/q, B2, A2/8, B2, C#3, D3', {clef: 'bass', stem: 'up'})),
            score.voice(score.notes('C#2/h, C#2', {clef: 'bass', stem: 'down'}))
        ],
    }).addClef('bass').addTimeSignature('4/4');
        
    system.addStave({
        voices: [
            score.voice(score.notes('C#3/q, B2, A2/8, B2, C#3, D3', {clef: 'bass', stem: 'up'})),
            score.voice(score.notes('C#2/h, C#2', {clef: 'bass', stem: 'down'}))
        ],
    }).addClef('bass').addTimeSignature('4/4');
        
    system.addConnector();


    vf.draw();
}

await main();