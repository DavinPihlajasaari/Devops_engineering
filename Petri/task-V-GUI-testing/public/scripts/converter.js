/**
 * Converts a hex color to RGB.
 *
 * @param {string} hex - Hex color, e.g. "#FF5733" or "#F53".
 * @returns {{ r: number, g: number, b: number }} RGB values.
 * @throws {TypeError} If input is not a string.
 * @throws {Error} If the hex color is invalid.
 */
export function hexToRgb(hex) {
  if (typeof hex !== "string") {
    throw new TypeError("Hex color must be a string");
  }

  // Remove the optional "#" prefix.
  let normalized = hex.startsWith("#") ? hex.slice(1) : hex;

  // Accept only 3- or 6-digit hexadecimal colors.
  if (!/^(?:[0-9a-fA-F]{3}|[0-9a-fA-F]{6})$/.test(normalized)) {
    throw new Error("Invalid hex color");
  }

  // Expand shorthand, e.g. "F53" -> "FF5533".
  if (normalized.length === 3) {
    normalized = normalized
      .split("")
      .map((char) => char + char)
      .join("");
  }

  // Convert the hex string to a number.
  const value = Number.parseInt(normalized, 16);

  // Extract each 8-bit RGB channel.
  return {
    r: (value >> 16) & 255,
    g: (value >> 8) & 255,
    b: value & 255,
  };
}