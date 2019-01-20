/* $VER: ReadMe2Guide 0.7 $
 * Convert simple ReadMe file to AmigaGuide
 * � Stefan Haubenthal 2010-2014
 * rx ReadMe2Guide <foo.txt >foo.guide foo.guide 1.0
 */

say '@database "'word(arg(1), 1)'"'
say '@author "'word(sourceline(1), 3) word(sourceline(1), 4)'"'
say '@node Main "Main"'
say '@toc Contents'
if arg() then say '@$VER:' arg(1) '('translate(date('e'),'.','/')') $'
if ~eof(stdin) then parse pull last "0d"x
prevnode="Main"
toc=""
do until eof(stdin)
	parse pull curr "0d"x
	if length(curr)>0 & (curr=copies("=", length(last)) | curr=copies("-", length(last))) then
	do
		if prevnode~="Main" then
		do
			say '@endnode'
			say '@node 'compress(last,' "/:')' "'compress(last,'"')'"'
			say '@toc Contents'
			toc=toc'@{"'compress(last,'"')'" link 'compress(last,' "/:')'}'"0a"x
		end
		prevnode=last
	end
	/* escape at-sign */
	if pos("@", last)>0 then
		say insert("\", last, pos("@", last)-1)
	else
		/* *bold* */
		do
		a=pos(" *", last)
		b=pos("* ", last)
		if b-a>2 then
			say delstr(insert("@{ub}", delstr(insert("@{b}", last, a), a+5, 1), b+2), b+8, 1)
		else
			/* _italic_ */
			do
			a=pos(" _", last)
			b=pos("_ ", last)
			if b-a>2 then
				say delstr(insert("@{ui}", delstr(insert("@{i}", last, a), a+5, 1), b+4), b+3, 1)
			else
				say last
			end
		end
	last=curr
end
say '@endnode'
say '@node Contents'
say toc'@endnode'
