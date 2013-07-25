#makeHTML.py version 0.8.1 September 10, 2008 Jerry Stratton
#
#part(code="tag" (defaults to paragraph), content="text" style="css style name", id="css id", attributes={named array of attributes for tag}
#	addAttribute(attributename="name for tag attribute", attributevalue="value for tag attribute")
#	addPart(code, content, style, id, attributes): adds at end
#	addPiece(thePart=another part object or "content text")
#	addPieces(pices=a list of part objects or content texts)
#	insertPart(code, content, style, id, attributes): inserts at start
#	insertPiece(thePart)
#	make(tab="initial tabs")
#	makePart(code, content, style, id, attributes): returns a part object
#	__len__: parts support the len() function; return number of pieces directly contained
#snippet(code (defaults to "em"), content, posttext="text that comes directly after tag", pretext="text that comes directly before tag", style, id, attributes)
#
#head(title="text for title of page")
#body(title="text for main headline", style, id, attributes)
#page(pieces, style, id, attributes)
#styleSheet(sheet="url of stylesheet", media="relevance of style sheet")
#
#headline(content="text content" (required), level="numerical level", style, id, attributes)
#
#table(rows=list of data for rows, style, thStyle="css style for table headers", tdStyle="css style for table cells",
#		trStyle="css style for table rows", tdBlankStyle="css style for blank cells", firstRowHeader=1--if first row is a header row,
#		firstColumnHeader=1--if first column is a header column, id, attributes)
#	addRow(rowList=[list of cells or data], celltype="th or td", cellclass="css style of cells", attributes, style)
#	addRows(rows=[list of list of cells or data], celltype, cellclass, attributes)
#	columnCount()
#tableByColumn(columns=[list of columns], style, thStyle, tdStyle, trStyle, tdBlankStyle, firstRowHeader, firstColumnHeader, id, attributes)
#	addColumn(columnList=[list of column data or cells], celltype, cellclass, attributes)
#	addColumns(columns=[list of list of columns or column data], celltype, cellclass, attributes)	
#tableColumn(column=[list of data or cells for column], celltype, cellclass, firstRowHeader, thStyle, tdBlankStyle, attributes)
#	addCell(cell="cell or cell content", celltype, cellclass, id, attributes)
#	addCells(column="list of cells or cell contents", celltype, cellclass, attributes)
#tableRow(celltype, row=[list of cells or cell data], style, cellclass, firstColumnHeader, thStyle, id, attributes)
#	addCell(cell="cell or cell content", celltype, cellclass, colspan="numerical span of cell vertically", rowspan="numerical span of cell horizontally")
#	addCells(cells=[list of cells or cell content])
#	columnCount()
#
#linkedList(links=[list of items of the form [url, name]], outer="outer html tag", inner="inner html tag", style="outer css style",
#		iclass="inner css style", id, attributes)
#	addLink(link=[url, name])
#	addLinks(links)
#simpleList(items=[list of text items], outer, inner, defaultname="text for marking default entry", default="text of default entry",
#		style, iclass, id, attributes)
#	addItem(item="text of item")
#	addItems(items)
#
#image(src="url of image", alt="alternate text for image", align="alignment", style, id, attributes)
#link(content, url, posttext, pretext, style, id, attributes)
#
#form(submitText="text of submit button", pieces, method="submit method", action="form action", submitName="name for submit button", submitAction="javascript for submission"
#		headline="headline text", headlineLevel (defaults to 2), style, attributes, id)
#input(type="type of form input", name="name for input", value="default value for input", size="size for input", maxlength="maximum characters accepted",
#		style, id, attributes)
#select(name, items, default, style, iclass, id, attributes
#textinput(name, text, value, size, maxlength, style, id, attributes, type, tableRow=true if this should be a tr row, otherwise it is a paragraph)

#basic parts
class part:
	def __init__(self, code="p", content=None, style=None, id=None, attributes=None):
		self.style = style
		self.id=id
		self.pieces = []
		self.code = code
		if attributes == None:
			self.attributes = {}
		else:
			self.attributes = attributes
		if isinstance(content, list):
			self.addPieces(content)
		elif content != None:
			self.addPiece(content)
	
	def __len__(self):
		return len(self.pieces)

	def addAttribute(self, attributename, attributevalue):
		self.attributes[attributename] = attributevalue

	def addPart(self, code='p', content=None, style=None, id=None, attributes=None):
		newPart = self.makePart(code, content, style, id, attributes)
		self.addPiece(newPart)

	def addPiece(self, thePart):
		self.pieces.append(thePart)
	
	def addPieces(self, theParts):
		if theParts != None:
			if isinstance(theParts, list):
				for part in theParts:
					self.addPiece(part)
			else:
				self.addPiece(theParts)
	
	def insertPart(self, code='p', content=None, style=None, id=None, attributes=None):
		newPart = self.makePart(code, content, style, id, attributes)
		self.insertPiece(newPart)
		
	def insertPiece(self, thePart):
		self.pieces.insert(0, thePart)

	def make(self, tab="\t"):
		startHTML = '<' + self.code
		
		if (self.attributes):
			for attribute in self.attributes:
				content = self.attributes[attribute]
				if content == None:
					startHTML += ' ' + attribute
				else:
					startHTML += ' ' + attribute + '="' + str(content) + '"'
	
		if (self.style):
			startHTML += ' class="' + self.style + '"'
		
		if (self.id):
			startHTML += ' id="' + self.id + '"'

		if self.pieces:
			startHTML += '>'
			
			partItems = [startHTML]
			
			if len(self.pieces) > 1:
				sep = "\n" + tab
				finalSep = sep[:-1]
				newtab = tab + "\t"
			else:
				newtab = tab
				sep = ""
				finalSep = ""
			
			for piece in self.pieces:
				if isinstance(piece, str):
					partItems.append(piece)
				elif isinstance(piece, int) or isinstance(piece, float):
					partItems.append(str(piece))
				elif piece == None:
					partItems.append("")
				else:
					partItems.append(piece.make(newtab))
		
			code = sep.join(partItems)
			code += finalSep + '</' + self.code + '>'
			return code
			
		else:
			startHTML += ' />'
			return startHTML
	
	def makePart(self, code='p', content=None, style=None, id=None, attributes=None):
		newPart = part(code, content, style, id, attributes)

		return newPart

class snippet(part):
	def __init__(self, code="em", content=None, posttext=None, pretext=None, style=None, id=None, attributes=None):
		part.__init__(self, code, content, style, id, attributes)
		self.posttext = posttext
		self.pretext = pretext
	
	def make(self, tab=''):
		snippets = []

		if self.pretext:
			snippets.append(self.pretext)
		
		snippets.append(part.make(self, tab))
		
		if self.posttext:
			snippets.append(self.posttext)

		return "".join(snippets)

#common body parts

class head(part):
	def __init__(self, title=None):
		part.__init__(self, code="head")
		if title:
			self.addPiece(part("title", title))

class body(part):
	def __init__(self, title=None, style=None, id=None, attributes=None):
		part.__init__(self, code="body", style=style, id=id, attributes=attributes)
		if title:
			self.addPiece(headline(title, 1))

class page(part):
	def __init__(self, pieces=None, style=None, id=None, attributes=None):
		part.__init__(self, code="html", style=style, id=id, attributes=attributes)
		if isinstance(pieces, list):
			self.addPieces(pieces)
		elif isinstance(pieces, part):
			self.addPiece(pieces)

	def make(self, tab=''):
		pageContent = part.make(self)
		
		print 'Content-type: text/html'
		print ''

		print pageContent

class styleSheet(part):
	def __init__(self, sheet, media='all'):
		attributes = {}
		attributes['rel'] = "StyleSheet"
		attributes['href'] = sheet + '.css'
		attributes['type'] = "text/css"
		attributes['media'] = media
		part.__init__(self, code="link", attributes=attributes)


#paragraph level parts
class headline(part):
	def __init__(self, content, level=2, style=None, id=None, attributes=None):
		code = "h"+str(level)
		part.__init__(self, content=content, style=style, code=code, id=id, attributes=attributes)



#tables
class table(part):
	def __init__(self, rows=None, style=None, thStyle=None, tdStyle=None, trStyle=None, tdBlankStyle=None, firstRowHeader=None, firstColumnHeader=None, id=None, attributes=None):
		part.__init__(self, code="table", style=style, id=id, attributes=attributes)
		self.rowclass=trStyle
		self.cellclass=tdStyle
		self.cellheaderclass=thStyle
		self.cellblankclass=tdBlankStyle
		self.firstRowHeader=firstRowHeader
		self.firstColumnHeader=firstColumnHeader
		if rows:
			self.addRows(rows)

	def addRow(self, rowList, celltype=None, cellclass=None, attributes=None, style=None):
		if cellclass==None:
			if self.firstRowHeader and len(self.pieces) == 0:
				celltype="th"
				cellclass=self.cellheaderclass
			else:
				cellclass=self.cellclass

		if style==None:
			style = self.rowclass

		newRow = tableRow(celltype, rowList, style, cellclass, self.firstColumnHeader, self.cellheaderclass, attributes)
		self.addPiece(newRow)
	
	def addRows(self, rows, cellclass=None, celltype=None, attributes=None):
		for row in rows:
			self.addRow(row)

	def columnCount(self):
		maxCount = 0
		for row in self.pieces:
			if isinstance(row, tableRow):
				colCount = row.columnCount()
				maxCount = max(maxCount, colCount)
		
		return maxCount

class tableByColumn(table):
	def __init__(self, columns=None, style=None, thStyle=None, tdStyle=None, trStyle=None, tdBlankStyle=None, firstRowHeader=None, firstColumnHeader=None, id=None, attributes=None):
		table.__init__(self, [], style, thStyle, tdStyle, trStyle, tdBlankStyle, firstRowHeader, firstColumnHeader, id, attributes)
		self.columns = []
		if columns:
			self.addColumns(columns)

	def addColumn(self, columnList, celltype=None, cellclass=None, attributes=None):
		if cellclass==None:
			if celltype == "th" or (self.firstColumnHeader and len(self.columns) == 0):
				celltype="th"
				cellclass=self.cellheaderclass
			else:
				cellclass=self.cellclass

		newColumn = tableColumn(columnList, celltype, cellclass, self.firstRowHeader, self.cellheaderclass, self.cellblankclass, attributes)
		self.columns.append(newColumn)

	def addColumns(self, columns, celltype=None, cellclass=None, attributes=None):
		for column in columns:
			self.addColumn(column)

	def addPiece(self, thePart):
		if isinstance(thePart, tableColumn):
			self.columns.append(thePart)
		else:
			part.addPiece(self, thePart)

	def make(self, tabs):
		rowCount = 0
		for column in self.columns:
			rowCount = max(rowCount, len(column.pieces))

		if rowCount:
			for cRow in range(rowCount):
				row = tableRow()
				for column in self.columns:
					if cRow < len(column.pieces):
						cell = column.pieces[cRow]
					else:
						cell = part("td")
					row.addPiece(cell)
				
				self.addPiece(row)

		myPart = part.make(self, tabs)
		return myPart


class tableColumn(part):
	def __init__(self, column=None, celltype="td", cellclass=None, firstRowHeader=None, thStyle=None, tdBlankStyle=None, attributes=None):
		part.__init__(self, "column", style=cellclass, attributes=attributes)
		if celltype==None:
			self.celltype = "td"
		else:
			self.celltype=celltype
		
		self.firstRowHeader=firstRowHeader
		self.cellheaderclass=thStyle
		self.cellblankclass=tdBlankStyle
		
		if column:
			self.addCells(column)

	def addCell(self, cell=None, celltype=None, cellclass=None, id=None, attributes=None):
		if self.cellblankclass and (cell==None or cell==""):
			celltype="td"
			cellclass = self.cellblankclass
		elif self.firstRowHeader and len(self.pieces) == 0:
			celltype = "th"
			cellclass = self.cellheaderclass
		else:
			if celltype == None:
				celltype = self.celltype
			if cellclass == None:
				cellclass = self.style
		if cell == None:
			cell = ""
		
		tableCell = part(code=celltype, style=cellclass, content=cell, id=id, attributes=attributes)
		self.addPiece(tableCell)

	def addCells(self, column, celltype=None, cellclass=None,  attributes=None):
		for cell in column:
			self.addCell(cell, celltype, cellclass, attributes=attributes)
	
	def make(self):
		print "Problem: columns should never be requested to make themselves."
		print "Columns are not true html structures, and should only be added to tableByColumn parts."


class tableRow(part):
	def __init__(self, celltype="td", row=None, style=None, cellclass=None, firstColumnHeader=None, thStyle=None, id=None, attributes=None):
		part.__init__(self, "tr", style=style, id=id, attributes=attributes)
		self.celltype=celltype
		self.cellclass=cellclass
		self.firstColumnHeader=firstColumnHeader
		self.cellheaderclass=thStyle
		if row:
			self.addCells(row)
		
	def addCell(self, cell, celltype=None, cellclass=None, colspan=None, rowspan=None):
		if self.firstColumnHeader and len(self.pieces) == 0:
			celltype="th"
			cellclass=self.cellheaderclass
		elif celltype == None:
			celltype = self.celltype
		
		if celltype == None:
			celltype = "td"
		
		if cellclass==None:
			cellclass=self.cellclass
		
		attributes = {}
		if colspan:
			attributes['colspan'] = colspan
		if rowspan:
			attributes['rowspan'] = rowspan
		
		if cell == None:
			cell = ""
		
		self.addPiece(part(code=celltype, style=cellclass, content=cell, attributes=attributes))

	def addCells(self, cells):
		for cell in cells:
			self.addCell(cell)

	def columnCount(self):
		return len(self.pieces)


#lists

class linkedList(part):
	def __init__(self, links=None, outer = "ul", inner="li", style=None, iclass=None, id=None, attributes=None):
		part.__init__(self, code=outer, style=style, id=id, attributes=attributes)
		self.innercode = inner
		self.innerstyle = iclass
		if isinstance(links, list):
			self.addLinks(links)

	def addLink(self, link):
		[url, name] = link
		link = part("a", attributes={"href": url}, content=name)
		listitem = part(self.innercode, content=link, style=self.innerstyle)
		self.pieces.append(listitem)
	
	def addLinks(self, links):
		theLinks = []
		for link in links:
			self.addLink(link)

class simpleList(part):
	def __init__(self, items=None, outer = "ul", inner="li", defaultname=None, default=None, style=None, iclass=None, id=None, attributes=None):
		part.__init__(self, code=outer, style=style, id=id, attributes=attributes)
		self.innercode = inner
		self.innerstyle = iclass
		self.defaultname = defaultname
		self.default=default
		if isinstance(items, list):
			self.addItems(items)
			
	def addItem(self, item):
		attributes = {}
		if self.defaultname:
			if item == self.default:
				attributes[self.defaultname] = None
	
		theItem = part(self.innercode, content=item, style=self.innerstyle, attributes=attributes)
		self.pieces.append(theItem)

	def addItems(self, items):
		theList = []
		for item in items:
			self.addItem(item)

#individual pieces

class image(part):
	def __init__(self, src, alt=None, align=None, style=None, id=None, attributes=None):
		if attributes == None:
			attributes = {}
		attributes['src'] = src
		if alt:
			attributes['alt'] = alt
		if align:
			attributes['align'] = align

		part.__init__(self, 'img', style=style, id=id, attributes=attributes)

class link(snippet):
	def __init__(self, content=None, url=None, posttext=None, pretext=None, style=None, id=None, attributes=None):
		if url != None:
			if attributes == None:
				attributes = {}
			attributes['href'] = url

		snippet.__init__(self, "a", content, posttext, pretext, style, id, attributes)
		

#forms

class form(part):
	def __init__(self, submitText="Submit", pieces=None, method="get", action='./', submitName = None, submitAction=None, headline=None, headlineLevel = 2, style=None, attributes=None):
		self.submitText = submitText
		self.submitName = submitName
		self.making = None
		if attributes == None:
			attributes = {}
		if action != None:
			attributes['action'] = action
		if method != None:
			attributes['method'] = method
		if submitAction != None:
			attributes['onSubmit'] = '"' + submitaction + '"'
		
		part.__init__(self, 'form', style=style, attributes=attributes)
		
		if headline != None:
			headcode = "h" + headlineLevel
			self.addPart(headcode, content=headline)
		
		self.addPieces(pieces)


	def make(self, tab=''):
		if self.making:
			return part.make(self, tab)
		else:
			if self.submitName:
				submitButton = input("submit", value=self.submitText, name=self.submitName)
			else:
				submitButton = input("submit", value=self.submitText)
			trueForm = self
			trueForm.making = 1
			trueForm.addPiece(submitButton)
			pageContent = trueForm.make(tab)
			return pageContent

class input(part):
	def __init__(self, type, name=None, value=None, size=None, maxlength=None, style=None, id=None, attributes=None):
		if attributes == None:
			attributes = {}

		attributes['type'] = type
		if name:
			attributes['name'] = name
		if value!=None:
			attributes['value'] = value
		if size:
			attributes['size'] = size
		if maxlength:
			attributes['maxlength'] = maxlength

		part.__init__(self, 'input', style=style, id=id, attributes=attributes)

class select(simpleList):
	def __init__(self, name, items=None, default=None, style=None, iclass=None, id=None, attributes=None):
		if attributes==None:
			attributes={}
		attributes['name'] = name

		simpleList.__init__(self, items, outer='select', inner='option', defaultname='selected', default=default, style=style, iclass=iclass, id=id, attributes=attributes)
		
class textinput(part):
	def __init__(self, name=None, text=None, value=None, size=None, maxlength=None, style=None, id=None, attributes=None, type="text", tableRow=None):
		if (text == None):
			text = name
		self.field = input(type=type, name=name, value=value, size=size, maxlength=maxlength, style=style, id=id, attributes=attributes)
		self.text = text
		
		if tableRow == None:
			part.__init__(self, 'p', style=style, attributes=attributes)
			self.addPiece(self.text)
			self.addPiece(self.field)
		else:
			part.__init__(self, 'tr', style=style, attributes=attributes)
			self.addPart('th', content=self.text)
			self.addPart('td', content=self.field)



#need some functions for HTML
#ought to be somewhere else in Python?
#cgi.escape only seems to do <, >, and &
from htmlentitydefs import entitydefs
import re

entitydefs_inverted = {}
for k,v in entitydefs.items():
	entitydefs_inverted[v] = k

needencoding = re.compile('|'.join(entitydefs.values()))
alreadyencoded = re.compile('&\w+;|&#[0-9]+;')

#encodes any special characters to their HTML equivalents
def encode(text, skip=None, once_only=1):
	# if extra_careful, check to see if this text has already been converted
	if not (once_only and alreadyencoded.findall(text)):
		if not isinstance(skip, list):
			skip = [skip]

		#do ampersands on their own or we might end up converting our conversions
		if '&' not in skip:
			text = text.replace('&','&amp;')
			skip.append('&')

		needlist= []
		#grab the characters in the text that need encoding
		for x in needencoding.findall(text):
			#and make sure we aren't skipping them
			if x not in skip:
				needlist.append(x)

		for uncoded in needlist:
			encoded = entitydefs_inverted[uncoded]
			#if it is not a numerical encoding, we need to do the & and ; ourselves
			if not encoded.startswith('&#'):
				encoded = '&%s;'%entitydefs_inverted[uncoded]
	
			text = text.replace(uncoded, encoded)

	return text
