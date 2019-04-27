module RedFloat
  VERSION = '0.1.0-dev'

  module Version
    numbers, TAG = VERSION.split("-")
    MAJOR, MINOR, MICRO = numbers.split(".").map(&:to_i)
    STRING = VERSION
  end
end
