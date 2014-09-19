BEGIN {
	i = 0;
	j = 0;
	k = 0;
}

/^ *#[^ ]+=".*"$/ { # Commented out attribute
	k++;
	sub(">", "", $1);
	sub("^#", "", $1);
	split($1, arr, "=");
	option = arr[1];
	sub("^"option": ", "", comment);
	options[i "," j "," k] = option;
	comments[i "," j "," k] = comment;
	comment = "";
}

/^ *# *[^ #|-]+#?/ { # Comment
	sub("^ *# +", "", $0);
	sub(" +#", "", $0);
	gsub("(^| )_+", "", comment);
	gsub("  +", " ", comment);
	comment = comment $0 " ";
}

/^(#-)+#? +[^#]+ +#?(-#)+-?$/ { # Section header
	max[i] = j;
	max[i "," j] = k;
	i++;
	j=0;
	k=0;
	sub("^(#-)+#?  *", "", $0);
	sub("  ?#?(-#)+-?$", "", $0);
	sections[i] = $0;
	comment = "";
}

/^</ { # Tag
	sub("<", "", $1);
	# If this tag is the same as the last, don't make a new section
	if (tags[i "," j] == $1) next;
	max[i "," j] = k;
	j++;
	k=0;
	tags[i "," j] = $1;
	if (j == 1) comments[i] = comment;
	comment = "";
}

/^ +[a-z0-9]+=".*">?$/ { # Attribute
	k++;
	sub(">", "", $1);
	split($1, arr, "=");
	option = arr[1];
	sub("^"option": ", "", comment);
	options[i "," j "," k] = option;
	comments[i "," j "," k] = comment;
	comment = "";
}

END {
	max[i] = j;
	max[i "," j] = k;
	sub("^.*/", "", FILENAME);
	sub(".example$", "", FILENAME);
	print "== " FILENAME " ==";
	max[0] = i;
	for (i=1; i<=max[0]; i++) {
		if (tags[i "," 1] == "die") continue; # If this is an RTFM section skip it
		if (max[i] == 0) continue; # If this is an empty section skip it
		print "=== " sections[i] " ===";
		if (comments[i]) print comments[i];
		print "";
		for (j=1; j<=max[i]; j++) {
			if (max[i "," j] == 0) continue;
			print "==== <" tags[i "," j] "> ====";
			for (k=1; k<=max[i "," j]; k++) {
				if (comments[i "," j "," k]=="") continue; # Skip attributes with no comments
				print "\t* " options[i "," j "," k] ": " comments[i "," j "," k];
			}
		}
		if (i != max[0]) print "\n";
	}
}
