--------------------------------------------------
-- My comment
-- second comment '
--comment one
	
select a,b,c, '--some stuff ;"' as "'mycol"
from mytbl -- some tbl
;

select [column  one /*  */], `col two /*  */`
from      [my  table]
;

/* second comment 
comment line 2
*/    

insert into tbl values (1,2,3);
--

/*comment*/    select a from tbl;

select some_stuff from tbl
-- last comment