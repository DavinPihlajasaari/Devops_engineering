// math.test.js

const { expect } = require("chai");
const { add, isEven } = require("../src/math");

/**
 * Unit tests for math.js
 *
 * @module test/math
 */
describe("math.js", () => {
  /**
   * Tests for the add() function
   */
  describe("add()", () => {
    /**
     * It should correctly add two numbers.
     */
    it("should add two numbers correctly", () => {
      expect(add(2, 3)).to.equal(5);
    });

    /**
     * It should throw an error if inputs are not numbers.
     */
    it("should throw an error if inputs are not numbers", () => {
      expect(() => add(2, "a")).to.throw("Inputs must be numbers");
    });
  });

  /**
   * Tests for the isEven() function
   */
  describe("isEven()", () => {
    /**
     * It should return true for even numbers.
     */
    it("should return true for even numbers", () => {
      expect(isEven(4)).to.be.true;
    });

    /**
     * It should return false for odd numbers.
     */
    it("should return false for odd numbers", () => {
      expect(isEven(5)).to.be.false;
    });

    /**
     * It should throw an error if the input is not a number.
     */
    it("should throw an error if input is not a number", () => {
      expect(() => isEven("test")).to.throw("Input must be a number");
    });
  });
});

