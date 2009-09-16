for($j=0;$j<8;$j++) {
	for($i=0;$i<10;$i++) {
		printf('<img src="out%d.png">', int(rand(8)));
	}
	printf("<br>\n");
}

print("<br>\n");
print('<img src="in.png"> -&gt; ');
for($i=0;$i<8;$i++) {
	printf('<img src="out%d.png"> ', $i);
}
printf("<br>\n");
