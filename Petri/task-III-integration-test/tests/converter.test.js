import { describe, it } from "mocha";
import { expect } from "chai";
import { hexToRgb } from "../src/converter.js";

// Group tests for the hexToRgb function.
describe("Converter Test:", () => {
  // Converts a standard six-digit hex color.
  it("converts a six-digit hex color to RGB", () => {
    expect(hexToRgb("#FF5733")).to.deep.equal({
      r: 255,
      g: 87,
      b: 51,
    });
  });

  // Accepts hex colors without the "#" prefix.
  it("accepts hex without the # prefix", () => {
    expect(hexToRgb("00FF00")).to.deep.equal({
      r: 0,
      g: 255,
      b: 0,
    });
  });

  // Expands shorthand hex colors to six digits.
  it("supports three-digit shorthand", () => {
    expect(hexToRgb("#F53")).to.deep.equal({
      r: 255,
      g: 85,
      b: 51,
    });
  });

  // Verifies that black produces zero for every channel.
  it("converts black correctly", () => {
    expect(hexToRgb("#000000")).to.deep.equal({
      r: 0,
      g: 0,
      b: 0,
    });
  });

  // Verifies that white produces the maximum RGB values.
  it("converts white correctly", () => {
    expect(hexToRgb("#FFFFFF")).to.deep.equal({
      r: 255,
      g: 255,
      b: 255,
    });
  });

  // Rejects characters that are not valid hexadecimal digits.
  it("throws an error for invalid hex characters", () => {
    expect(() => hexToRgb("#GGGGGG"))
      .to.throw("Invalid hex color");
  });

  // Rejects hex colors with unsupported lengths.
  it("throws an error for unsupported lengths", () => {
    expect(() => hexToRgb("#12345"))
      .to.throw("Invalid hex color");
  });

  // Ensures non-string inputs trigger a TypeError.
  it("throws TypeError for non-string input", () => {
    expect(() => hexToRgb(123456))
      .to.throw(TypeError);
  });
});