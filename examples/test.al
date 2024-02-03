#Test of string concatenation

write "Single String"
write "Simple string" + " concat"

a = "Concat "
write a + " one variable left"

a = "one variable right"
write "Concat " + a

a  = "Concat 2 "
b  = "Variables"

write a + b

#Test of string repetition

write "Right"*2
write 2*"Left"
a = "A"
b = 2
write a * b
write b * a

#Test of undefined operations, should output warnings
write a + 2
write a / 2
write b/0
