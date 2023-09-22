`bigfib`: prints the *n*th Fibonacci number, fast, with arbitrary precision.

Roughly equivalent to, but slightly faster than, the following:

```ruby
#!/usr/bin/env ruby
require 'matrix'

def fib(n)
  (Matrix[[1, 1], [1, 0]] ** n)[0, 0]
end

n = ARGV[0].to_i
puts fib(n)
```
