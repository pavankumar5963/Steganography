# Steganography
LSB image steganography in C 


Abstarct:
Steganography is the art of hiding the fact that communication is taking place, by hiding
information in other information. Many different carrier file formats can be used, but
digital images are the most popular because of their frequency on the internet. For hiding
secret information in images, there exists a large variety of steganography techniques some
are more complex than others and all of them have respective strong and weak points.
Different applications may require absolute invisibility of the secret information, while
others require a large secret message to be hidden.
Steganography is the practice of hiding private or sensitive information within something
that appears to be nothing out to the usual. Steganography is often confused with
cryptology because the two are similar in the way that they both are used to protect
important information. The difference between two is that steganography involves hiding
information so it appears that no information is hidden at all. If a person or persons views
the object that the information is hidden inside of he or she will have no idea that there is
any hidden information, therefore the person will not attempt to decrypt the information.
What steganography essentially does is exploit human perception, human senses are not
trained to look for files that have information inside of them, although this software is
available that can do what is called Steganography. The most common use of steganography
is to hide a file inside another file.


Requirements:
1)	The application accept an image file say .bmp along with the a text file which
	contains the message to be steged
2)	Analize the size of the message file and the data part of the .bmp file to check
	whether the messsage could fit in the provided .bmp image
3)	Provide a option to steg a magic string which could be usefull to identify whether the
	image is steged or not
4)	The appliaction should provide a option to decrpt the file
5)	This has to be an command line apliaction and all the options has to be passed as an
	command line argument
