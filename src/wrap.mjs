globalThis.print = cpp_print;

globalThis.console = {
    log(txt) {
        print(txt);
    },
    warn(txt) {
        print(txt);
    },
    error(txt) {
        print(txt);
    },
};

globalThis.assert = function assert(value) {
    if (!value) {
        throw new Exception("Assertion failed");
    }
};

globalThis.window = {
    addEventListener() {},
    setTimeout() {},
};

globalThis.document = {
    getElementById(id) {
        console.log(`getElementById(${id})`);
        return {}
    },
};

class Node {
    static ELEMENT_NODE=1
}
globalThis.Node = Node;

export default {}
