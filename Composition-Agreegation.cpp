
Composition                   Agreegation
Stronger relationship         Weak Relationship
no sharing                    sharing can be
Life cycle limited            Not limited
Department cannot belong 
to more than one university   Prof can be part of more than 1 department

Composition is even stronger relationship than aggregation. 
To test if we are dealing with composition, use no sharing rule for composition:
part can belong to only one whole.

Besides the no sharing rule, another the rule for composition is: if the whole is destroyed,
is the part that makes it up also destroyed?

Code Example: A university owns various departments, and each department has a number of professors. 
If the university closes, the departments will no longer exist, but the professors in those departments
will continue to exist. Therefore, a University can be seen as a composition of departments, 
whereas departments have an aggregation of professors. 
In addition, a Professor could work in more than one department, but a department could not be part
of more than one university.

University - Department : composition: ownership: destroyed with the whole: strong has a relationship 

The composite object takes ownership of the component. This means the composite is responsible 
for the creation and destruction of the component parts. An object may only be part of one composite. 
If the composite object is destroyed, all the component parts must be destroyed.
Composition enforces encapsulation as the component parts usually are members of the composite object:

Department - Professors: aggregation: no ownership: may outlive: component may still have weak has a relationship 
