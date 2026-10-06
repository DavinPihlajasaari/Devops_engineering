const mylib = require("./mylib");

const a = 10;
const b = 2;

try {
    console.log("Addition:", mylib.addition(a, b));
    console.log("Subtraction:", mylib.subtraction(a, b));
    console.log("Multiplication:", mylib.multiplication(a, b));
    console.log("Division:", mylib.division(a, b));
} catch (error) {
    console.error("Error:", error.message);
}
