/*
 * neuronTrace.js
 * Requests connectome data from server and generates the neuron trace maps.
 * 
 * Dependencies:	jquery.js
 * 					d3.js
 * 					aux.js	
 * 				
 * Created by: Christopher Brittin 05/2012
 * 
 * Updated:  
 * 
 *
 * NOTE BY ANDREW (10/18/16):
 *
 * http://129.98.110.239/data/dataTools/php/retrieve_coord.php?neuron=PDB&db=JSE
 * 129.98.110.239 is/was wormwiring's ip address (their dns seems to be down). 
 * 
 *
 * the link above provides data that seems to show where synapses are made with the given neuron, with respect to section number. This data is seen on wormwiring's normal site in the 'graph' area (see link below)
 * 
 * http://129.98.110.239/data/neuronData.php?name=PDB&db=JSE
 *
 * This should allow us to figure out where different neurons have synapses, along with their lengths, relative to eachother.
 * 
 * From capturing headers while loading the second link (to the normal wormwiring stuff), the GET request in the first link was found, which seems to fit with the request that this file will send in order to get a 'NeuronTrace'. 
 *
 */
 
var non_series_keys = array_to_object(["plotParam","cellBody","preSynapse","postSynapse","gapJunction","remarks","nmj"]);
var delta_min_max = 10;

TestNeuronTrace = function (elemId,neuron,dim){
	//Toy data
	var dataset = {
		VC: { x: [[5000,5000],[5000,5010],[5010,5030],[5010,3000],[5030,5030],[3000,4070]],
				y: [[5000,5000],[5000,5010],[5010,5030],[5010,4090],[5030,5030],[4090,4070]],
				z: [[1300,1301],[1301,1302],[1302,1303],[1302,1303],[1303,1304],[1303,1304]]},
		PAG: { x: [[5030,5040],[5040,5020],[5020,5050]],
				y: [[5030,5040],[5040,5020],[5020,5050]],
				 z: [[1304,1305],[1305,1306],[1306,1307]]},
		plotParam: {xScaleMin:0,
					xScaleMax:6500,
					yScaleMin: 0,
					yScaleMax: 6500,
					zScaleMin: 1300,
					zScaleMax: 1307},
		cellBody: { x: [[5000,5000],[5000,5010],[5010,5030]],
		            y: [[5000,5000],[5000,5010],[5010,5030]],
					z: [[1300,1301],[1301,1302],[1302,1303]]
				},
		remarks:[[4070,4070,1304,'VC','A'], [5050,5050,1307,'PAG','B'], [5000,5000,1300,'VC','end']],
		preSynapse: [[5000,5000,1301,4,'LUAL,LUAR',1301,1302], [5030,5030,1303,6,'AVAL',1303,1303]],
		postSynapse:[[4070,4070,1304,1,'AVAR',1304,1304]],
		gapJunction:[[5020,5020,1306,3,'AVDL',1306,1306]]
	};
	
	return new NeuronTrace(elemId,neuron,dataset,dim,"null");
	
	 
 };

CallNeuronTrace = function(elemId,neuron,dim,db,url){
	//var db = "n2ywindows";
	//alert(db + " --> " + neuron + " --> " + dim );
	$.ajax({
			type: "GET",
			url: url,
			data: {"neuron": neuron,"db":db},
			dataType: "json",
			success: function(dataset){
				d3.select("#trace_container").remove();
				d3.select("#wrap").append("div")
					.attr("class","trace")
					.attr("id","trace_container");								
				new NeuronTrace(elemId,neuron,dataset,dim,db);
			},
			error: function(){
				alert("There was an error connecting to sever. Please notify admin.");
			}
		});
	
};

Array.prototype.last = function(){
	return this[this.length-1];
}

NeuronTrace = function(elemId,neuron,dataset,dimension,db){
	console.log(dataset);
	var self = this;
	//flags
	this.syn_flag = {
		preSynapse: true,
		postSynapse: true,
		gapJunction: true,
		synLabel:true
	};		
	this.chart = document.getElementById(elemId);
	this.cx = this.chart.clientWidth;
	this.cy = this.chart.clientHeight;
	this.title = neuron || new String();
	this.neuron = neuron;
	this.db = db;
	this.data = dataset || {};
	this.plotParam = {
		xmin: dataset.plotParam.xScaleMin,
		xmax: dataset.plotParam.xScaleMax,
		ymin: dataset.plotParam.yScaleMin,
		ymax: dataset.plotParam.yScaleMax,
		zmin: dataset.plotParam.zScaleMin,
		zmax: dataset.plotParam.zScaleMax		
				};
	
	this.dim = dimension || "yz";
	if (this.dimension == "yz"){
		this.ylabel = ["Ventral","Dorsal"];
	} else if (this.dimension == "xz") {
		this.ylabel = ["Left","Right"];
	};	
	this.tooltipoffX = 100;
	this.tooltipoffY = -80;
	this.padding = {
		"top": this.title ? 40 : 20,
		"right": 170,
		"bottom": 75,
		"left": 75,
	};
	//consider updating php script to get rid of this
	this.sec_rec = [];
	var synapse_data = this.format_synapses();
	this.syn_data = synapse_data.array
	this.syn_max = synapse_data.synMax;
	this.synThresh = 0;
	
	this.size = {
		"width": this.cx - this.padding.left - this.padding.right,
		"height": this.cy - this.padding.top - this.padding.bottom
	};
	
	
	//z -scale 
	this.z = d3.scale.linear()
		.domain([this.plotParam.zmin,this.plotParam.zmax])
		.range([0,this.size.width])
	
	//get number of ticks
	this.getNumTicks();		
	
	
	
	//Outer frame
	this.canvas = d3.select(this.chart).append("svg")
		.attr("class","canvas")
		.attr("width", this.cx)
		.attr("height", this.cy)
		
	this.frame = this.canvas.append("g")
					.attr("class","frame")
					.attr("transform", "translate(" + this.padding.left + "," + this.padding.top + ")");
	
	this.create_frame();
	
	//Define tooltip
	this.syn_tooltip = d3.select(this.chart)
						.append("div")
						.attr("id","syn_tooltip")
						.style("position", "absolute")
						.style("z-index", "10")
						.style("visibility", "hidden")
						.text("Null")

	
	
	this.create_trace(this.dim);
};

NeuronTrace.prototype.create_frame = function(){
	this.add_chart_title();
	this.add_zaxis_label();
	this.add_series_legend();
	this.add_synapse_legend();
	this.dim_choise();
	this.synapse_threshold();
};

NeuronTrace.prototype.add_chart_title = function(){
		// add Chart Title
	this.frame.append("text")
		.attr("class", "mapTitle")
		.text(this.title)
		.attr("x", this.size.width/2)
		.attr("dy","-0.8em")
		.style("text-anchor","middle");
};

NeuronTrace.prototype.add_zaxis_label = function(){
	//add z-axis label	
	this.frame.append("text")
		.attr("class", "zlabel")
		.text("Section Number")
		.attr("x", this.size.width/2)
		.attr("y", this.size.height)
		.attr("dy","4.4em")
		.style("text-anchor","middle");
	this.frame.append("text")
		.attr("class", "zlabel")
		.text("<-- Anterior")
		.attr("x", 0)
		.attr("y", this.size.height)
		.attr("dy","4.4em")
		.attr("dx","4.4em")
		.style("text-anchor","middle");
	this.frame.append("text")
		.attr("class", "zlabel")
		.text("Posterior -->")
		.attr("x", this.size.width)
		.attr("y", this.size.height)
		.attr("dy","4.4em")
		.attr("dx","-4.4em")
		.style("text-anchor","middle");
};

NeuronTrace.prototype.add_series_legend = function(){
	var series = new Array();
		i = 0;
	for (var key in this.data) {
		if (!( key in non_series_keys)){	
			series[i] = key;
			i += 1;
		};
	};
	
	this.frame.append("svg:text")
			.text("Data Series:")
			.attr("class","seriesLabel")
			.attr("y",0)
			.attr("x",this.size.width+13)
	
	this.frame.append("svg:line")
			.attr("class","seriesLabel")
			.attr("x1",this.size.width+13)
			.attr("x2",this.size.width+115)
			.attr("y1",7)
			.attr("y2",7);
	
	this.frame.append("svg:text")
		.text("Cell Body")
		.attr("class","CellBody")
		.attr("x",this.size.width+13)
		.attr("y",35);
	
	this.frame.selectAll("path.text")
			.data(series)
			.enter()
			.append("svg:text")
			.text(function (d) {return d;})
			.attr("class",function (d) {return d;})
			.attr("x",this.size.width+13)
			.attr("y",function(d){
				var i = series.indexOf(d);
				return 35 + 25*(i+1);
			});
		
};

NeuronTrace.prototype.add_synapse_legend = function(){
	 var self = this;
	 
	 this.frame.append("svg:text")
			.text("Synapse:")
			.attr("class","synapseLabel")
			.attr("y",this.size.height/2+20)
			.attr("x",this.size.width+13);
	
	this.frame.append("svg:line")
			.attr("class","synapseLabel")
			.attr("x1",this.size.width+13)
			.attr("x2",this.size.width+95)
			.attr("y1",this.size.height/2 + 27)
			.attr("y2",this.size.height/2 + 27);
		
	this.frame.append("svg:text")
			.text("Chemical Output")
			.attr("class","preSynapse")
			.attr("y",this.size.height/2 + 55)
			.attr("x",this.size.width+13)
	        .style("cursor", "pointer")
	        .on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
			.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})
			.on("click",function(d) { 
				if (self.syn_flag["preSynapse"]) {
					d3.selectAll("circle.preSynapse").remove();
					d3.selectAll("text#preSynapseLabel").remove();
					d3.select(this).style("fill","#DEDADA");
					self.syn_flag["preSynapse"] = false;
				} else {
					self.syn_flag["preSynapse"] = true;
					d3.selectAll("text#postSynapseLabel").remove();
					d3.selectAll("text#gapJunctionLabel").remove();
					d3.select(this).style("fill"," #FA5882")
					self.update_synapses(self.dim,self.synThresh);					
				};
				});
	
	this.frame.append("svg:text")
			.text("Chemical input")
			.attr("class","postSynapseBtn")
			.attr("y",this.size.height/2 + 80)
			.attr("x",this.size.width+13)
	        .style("cursor", "pointer")
	        .on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
			.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})
			.on("click",function(d) { 
				if (self.syn_flag["postSynapse"]) {
					d3.selectAll("text#postSynapseLabel").remove();
					d3.selectAll("circle.postSynapse").remove();
					d3.select(this).style("fill","#DEDADA");
					self.syn_flag["postSynapse"] = false;
				} else {
					self.syn_flag["postSynapse"] = true;
					d3.selectAll("text#preSynapseLabel").remove();
					d3.selectAll("text#gapJunctionLabel").remove();
					d3.select(this).style("fill","#BF00FF")
					self.update_synapses(self.dim,self.synThresh);					
				};
				});
					
	this.frame.append("svg:text")
			.text("Gap junction")
			.attr("class","gapJunctionBtn")
			.attr("y",this.size.height/2 + 105)
			.attr("x",this.size.width+13)
	        .style("cursor", "pointer")
	        .on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
			.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})
			.on("click",function(d) { 
				if (self.syn_flag["gapJunction"]) {
					d3.selectAll("text#gapJunctionLabel").remove();
					d3.selectAll("circle.gapJunction").remove();
					d3.select(this).style("fill","#DEDADA");
					self.syn_flag["gapJunction"] = false;
				} else {
					self.syn_flag["gapJunction"] = true;
					d3.selectAll("text#preSynapseLabel").remove();
					d3.selectAll("text#postSynapseLabel").remove();
					d3.select(this).style("fill"," #00FFFF")
					self.update_synapses(self.dim,self.synThresh);					
				};
				});
	
	this.frame.append("svg:text")
			.text("Toggle labels")
			.attr("class","synLabelBtn")
			.attr("y",this.size.height/2 + 130)
			.attr("x",this.size.width+13)
	        .style("cursor", "pointer")
	        .on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
			.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})
			.on("click",function(d) { 
				if (self.syn_flag["synLabel"]) {
					d3.selectAll("text#preSynapseLabel").remove();
					d3.selectAll("text#postSynapseLabel").remove();
					d3.selectAll("text#gapJunctionLabel").remove();
					d3.select(this).style("fill","#DEDADA");
					self.syn_flag["synLabel"] = false;
				} else {
					self.syn_flag["synLabel"] = true;
					d3.select(this).style("fill","#000000");
					self.update_synapses(self.dim,self.synThresh);					
				};
				});
							
	this.syn_flag = self.syn_flag;		
};

NeuronTrace.prototype.dim_choise = function(){
	var self = this;
	
	this.frame.append("text")
		.attr("class", "zyBtn")
		.text("Z-Y")
		.attr("x", 15)
		.attr("dy","-0.8em")
		.style("text-anchor","middle")
		.attr("fill",function(){
			if (self.dim == "yz"){
				return "#000000";
			} else if (self.dim == "xz") {
				return "#DEDADA";
			}
		})
		.style("cursor", "pointer")
		.on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
		.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})	
		.on("click", function(d) {
			if (self.dim == "xz") {
				self.dim = "yz";
				d3.select(".zyBtn").attr("fill", "#000000");
				d3.select(".zxBtn").attr("fill","#DEDADA");
				d3.select("g.xz").remove();
				self.create_trace("yz");
				
			};
		}); 
	
	this.frame.append("text")
		.attr("class", "zxBtn")
		.text("Z-X")
		.attr("x", 65)
		.attr("dy","-0.8em")
		.style("text-anchor","middle")
			.attr("fill",function(){
				if (self.dim == "yz"){
					return "#DEDADA";
				} else if (self.dim == "xz") {
					return "#000000";
					
				}
			})
		.style("cursor", "pointer")
		.on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
		.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})
		.on("click", function(d) {
			if (self.dim == "yz") {
				self.dim = "xz";
				d3.select(".zxBtn").attr("fill", "#000000");
				d3.select(".zyBtn").attr("fill","#DEDADA");
				d3.selectAll("g.yz").remove();
				self.create_trace("xz");
			};
		}); 
	
	this.dimension = self.dim;
};

NeuronTrace.prototype.synapse_threshold = function(){
	var self = this;
	
	function printInput(self,value){
		self.frame.append("svg:text")
			.text(value)
			.attr("id","synThreshInput")
			.style("font-family","sans-serif")
			.style("font-size","14px")
			.attr("y",self.size.height/2 + 200)
			.attr("x",self.size.width+90);		
	};

	this.frame.append("svg:text")
		.text("Synapse threshold:")
		.attr("class","synThreshHead")
		.attr("y",this.size.height/2 + 160)
		.attr("x",this.size.width+13)
	
	this.frame.append("svg:line")
		.attr("class","synThreshHead")
		.attr("x1",this.size.width+13)
		.attr("x2",this.size.width+140)
		.attr("y1",this.size.height/2 + 167)
		.attr("y2",this.size.height/2 + 167);
	
	this.frame.append("svg:text")
		.text("# of sects:")
		.style("font-family","sans-serif")
		.style("font-size","14px")
		.attr("y",this.size.height/2 + 200)
		.attr("x",this.size.width+13);

	printInput(self,0);
	
	d3.select(this.chart).append("input")
		.style("position","absolute")
		.style("top",(this.size.height+25)+"px")
		.style("left",(this.size.width+90)+"px")
		.style("width","120px")
		.attr("id","synThresh")
		.attr("type","range")
		.attr("min",0)
		.attr("max",this.syn_max + 1)
		.attr("value",0)
		.on("change", function(){
			var value = this.value;
			d3.select("#synThreshInput").remove();
			printInput(self,value);
			d3.selectAll("circle.preSynapse").remove();
			d3.selectAll("text#preSynapseLabel").remove();
			d3.selectAll("text#postSynapseLabel").remove();
			d3.selectAll("circle.postSynapse").remove();
			d3.selectAll("text#gapJunctionLabel").remove();
			d3.selectAll("circle.gapJunction").remove();
			self.update_synapses(self.dim,value);
			self.update_synThresh(value);		
		});
};

NeuronTrace.prototype.update_synThresh = function(thresh){
	this.synThresh = thresh;
};

NeuronTrace.prototype.create_trace = function(dim){
	var self = this;
	//y-scale (x-scale)
	this.y = this.yscale(dim);	
	
	this.vis = this.canvas.append("g")
				.attr("class",self.dim)
				.attr("transform", "translate(" + this.padding.left + "," + this.padding.top + ")");	
	
	//add y-axis label
	this.add_ylabel(dim);
	
	this.plot = this.vis.append("rect")
      .attr("width", this.size.width)
      .attr("height", this.size.height)
      .style("fill", "#2F2F33")
      .style("cursor","move")
	  .attr("pointer-events", "all")
      .attr("stroke-width", "1px")
      .attr("stroke","black")
      this.plot.call(d3.behavior.zoom().x(this.z).y(this.y).on("zoom", this.redraw(dim)));

	//initial trace 
	this.trace = this.vis.append("svg")
      .attr("top", 0)
      .attr("left", 0)
      .attr("width", this.size.width)
      .attr("height", this.size.height)
      .attr("viewBox", "0 0 "+this.size.width+" "+this.size.height)
      .attr("class", "trace");

	this.update_trace(false,dim);	
	this.redraw(dim)();

};

NeuronTrace.prototype.format_synapses = function(){
	var self = this;	
	var syn_data = {
			synMax:0,
			array:[]
		};
	for (var key in array_to_object(["preSynapse","postSynapse","gapJunction"])) {
		for (i = 0; i < self.data[key].length; i++){
				var temp = self.data[key][i].concat([key]);
				//syn_array.push(temp);
				syn_data.array.push(temp);
				syn_data.synMax = Math.max(syn_data.synMax,temp[3]);
			};
				
	};
	//console.log(syn_data);
	return syn_data;
};

NeuronTrace.prototype.getNumTicks = function(){
	this.numTicks = 0,
		dz = this.plotParam.zmax - this.plotParam.zmin;
	if (dz <= 10) {this.numTicks = dz + 1}
	else if (dz <= 20) {this.numTicks = Math.round(dz/5) + 1}
	else if (dz <= 100) {this.numTicks = Math.round(dz/10) + 1}
	else if (dz <= 200) {this.numTicks = Math.round(dz/25) + 1}
	else if (dz <= 500) {this.numTicks = Math.round(dz/50) + 1}
	else {this.numTicks = Math.round(dz/100) + 1};
};

NeuronTrace.prototype.yscale = function(dim){
	if (dim == "yz"){
		var min = this.plotParam.ymin,
			max = this.plotParam.ymax;
	} else if (dim == "xz") {
		var min = this.plotParam.xmin,
			max = this.plotParam.xmax; 
	};
	
	return d3.scale.linear()
		.domain([min,max])
		.range([0,this.size.height]);
};

NeuronTrace.prototype.add_ylabel = function(dim){
	var self = this;
	if (dim == "yz"){
		var label1 = "Ventral",
			label2 = "Dorsal";
	} else if (dim == "xz"){
		var label1 = "Left",
			label2 = "Right";
	};
			
	this.vis.append("text")
		.attr("class", "axes")
		.text(label1)
		.attr("x", -this.padding.left/2)
		.attr("y", this.size.height)
		//.attr("dx","-3em")
		.style("text-anchor","middle");
	this.vis.append("text")
		.attr("class", "axes")
		.text(label2)
		.attr("x", -this.padding.left/2)
		.attr("y", 0)
		//.attr("dx","-3em")
		.style("text-anchor","middle");
	this.vis.append("line")
		.attr("class","axes")
		.attr("y1",7)
		.attr("y2",this.size.height-13)
		.attr("x1",-this.padding.left/2)
		.attr("x2",-this.padding.left/2);
};

NeuronTrace.prototype.update_trace = function(update_path,dim){
	var self = this;
	//loop through the different series
 	for (var key in self.data) {
		if (!( key in non_series_keys)){		
			//combine ('zip') yz data
			if (dim == "yz"){
				var yz = new zip([self.data[key].z,self.data[key].y]);
			} else if (dim == "xz"){
				var yz = new zip([self.data[key].z,self.data[key].x]);
			};
			self.add_path(yz,key,update_path);
					
		}
		
	};
	//Add cell body points

	
	if (self.data.cellBody.z.length > 0){
		var cb;
		if (dim == "yz"){
			cb = zip([self.data.cellBody.z,self.data.cellBody.y]);
		} else if (dim == "xz"){
			cb = zip([self.data.cellBody.z,self.data.cellBody.x]);
		};
		self.add_path(cb,'CellBody',update_path);
	};
	
	self.add_remarks(dim,self.data.remarks);
};

NeuronTrace.prototype.add_path = function(yz,key,update_path){
	var d = [];
	for (i = 0; i < yz.length; i++){
		d.push("M",this.z(yz[i][0][0]),this.y(yz[i][1][0]),this.z(yz[i][0][1]),this.y(yz[i][1][1]));
	};
	var d_path = d.join(" ");
	if (update_path == true){
		this.vis.select("path."+key).attr("d", d_path);
	} else {
		this.trace.append("path")
			.attr("class",key)
			.attr("d",d_path);
	};
};

NeuronTrace.prototype.add_remarks = function(dim,remarks){
	var self = this,
		idx;
	
	if (dim == "yz"){
		idx = 1;
	} else if (dim == "xz"){
		idx = 0;
	};
	
	var text = this.vis.select("svg").selectAll("text#remarks")
				.data(remarks);
	
	text.enter().append("text")
		.text(function(d){ return d[4];})
		.attr("class",function(d){ return d[3];})
		.attr("id","remarks")
		.attr("x",function(d){return self.z(d[2]);})
		.attr("y",function(d){return self.y(d[idx]);})
		.attr("dy",20)
		.attr("dx",5);					
		
	text
		.text(function(d){ return d[4];})
		.attr("class",function(d) { return d[3];})
		.attr("id","remarks")
		.attr("x",function(d){return self.z(d[2]);})
		.attr("y",function(d){return self.y(d[idx]);})
		.attr("dy",20)
		.attr("dx",5);		

		
	text.exit().remove();		

};

NeuronTrace.prototype.update_synapses = function(dim,thresh) {
	var self = this;
	var idx;
	
	if (dim == "yz"){
		idx = 1;
	} else if (dim == "xz"){
		idx = 0;
	};
	var temp_rec = [];
	var syn_temp = [];
	this.syn_temp = thresh;
	for (i = 0; i < self.syn_data.length; i++){
		if (this.syn_flag[self.syn_data[i].last()] && (self.syn_data[i][3] >= thresh)){
			syn_temp.push(self.syn_data[i]);
			temp_rec.push(self.syn_data[i][3]);
		};
	};
	//console.log(self.syn_data);
	this.syn_rec = temp_rec;
	var circle = this.vis.select("svg").selectAll("circle")
      .data(syn_temp);
      
     circle.enter().append("circle")
		.attr("class",function(d){return d.last()})
		.attr("cx",function(d){return self.z(d[2]);})
		.attr("cy",function(d){return self.y(d[idx]);})
		.attr("r",function(d){
			if (d[3] <=5 ) {
				return 4;
			} else if (d[3] <= 10){
				return 5;
			} else if (d[3] <= 20){
				return 7;
			} else {
				return 9;
			};
			return d[3];
		})
		.style("cursor", "pointer")
		.on("mouseover", function(d){
				txt = "(x,y,z) = " + "(" + d[0] + "," + d[1] + "," + d[2] + ")";
				if (d.last() == "preSynapse"){
					txt = self.neuron + " => " + d[4] +"\n";
				} else if (d.last() == "postSynapse"){
					console.log(d);
					txt = d[4] + " => " + self.neuron + "\n";
				} else if (d.last() == "gapJunction"){
					txt = self.neuron + " |=| " + d[4] + "\n";
				};
				txt = txt + "<br/> # sects: " + d[3] + "<br/> z-range: (" + d[5] + "," + d[6] + ")";
				txt = txt + "<br/> Contin #: " + d[7];
				return self.syn_tooltip.style("visibility", "visible").html(txt);
		})
		.on("mousemove", function(){
			var mousexy = d3.svg.mouse(self.vis.node());
			return self.syn_tooltip.style("top", (mousexy[1] + self.tooltipoffY) +"px").style("left",(mousexy[0] + self.tooltipoffX) + "px");
			})
		.on("mouseout", function(){return self.syn_tooltip.style("visibility", "hidden");})
		.on("click",function(d){
				var url = "http://wormwiring.org/data/getImages.php?neuron="+self.neuron+"&db="+self.db+"&continNum="+d[7];
				window.open(url,'_blank');
				window.focus();
				});
   
     circle
		.attr("class",function(d){return d.last();})
		.attr("cx",function(d){return self.z(d[2]);})
		.attr("cy",function(d){return self.y(d[idx]);})
		.attr("r",function(d){
			if (d[3] <=5 ) {
				return 4;
			} else if (d[3] <= 10){
				return 5;
			} else if (d[3] <= 20){
				return 7;
			} else {
				return 9;
			};
			return d[3];
		})
		
	circle.exit().remove();

	if (self.syn_flag["synLabel"]){
		var text = this.vis.select("svg").selectAll("text.indSynLabel")
							.data(syn_temp);	
		
		text.enter().append("text")
			.text(function(d) {
				if (d.last() == "preSynapse" || d.last() == "nmj"){
					txt = " => " + d[4] +"\n";
				} else if (d.last() == "postSynapse" || d.last() == "nmj"){
					txt = " <= " + d[4] + "\n";
				} else if (d.last() == "gapJunction"){
					txt = " |=| " + d[4] + "\n";
				};	
				return txt;
			})
			.attr("class","indSynLabel")
			.attr("id",function(d){return d.last() + "Label";})
			.attr("x",function(d){return self.z(d[2]);})
			.attr("y",function(d){return self.y(d[idx]);})
			.attr("dy",0)
			.attr("transform",function(d){
					return "rotate(270 " + self.z(d[2]) + "," + self.y(d[idx]) + ")";
					})
			.attr("dy",3)
			.attr("dx",10)

		
		text
			.attr("class","indSynLabel")
			.attr("id",function(d){return d.last() + "Label";})
			.attr("x",function(d){return self.z(d[2]);})
			.attr("y",function(d){return self.y(d[idx]);})
			.attr("dy",0)
			.attr("transform",function(d){
					return "rotate(270 " + self.z(d[2]) + "," + self.y(d[idx]) + ")";
					})
			.attr("dy",3)
			.attr("dx",10)

			
		text.exit().remove();
	};



};

NeuronTrace.prototype.update = function(dim){
	this.update_trace(true,dim);
	this.update_synapses(dim,this.synThresh);	
};

NeuronTrace.prototype.redraw = function(dim){
	var self = this;
	return function() {
		var tz = function(d) {
			return "translate(" + self.z(d) + ",0)";
		},
		stroke = function(d) {
			return "#000000"
		},
		fz = self.z.tickFormat(self.numTicks);
		
		var gz = self.vis.selectAll("g.z")
				.data(self.z.ticks(self.numTicks),String)
				.attr("transform",tz);
		gz.select("text")
			.text(fz);
				var gze = gz.enter().insert("g","a")
				.attr("class","z")
				.attr("transform", tz);
		
		gze.append("line")
			.attr("stroke", stroke)
			.attr("y1", self.size.height)
			.attr("y2", self.size.height + 10);
		
		gze.append("text")			
			.attr("class", "axis")
			.attr("y", self.size.height + 15)
			.attr("dy", "1em")
			.attr("font-family", "sans-serif")
			.attr("font-size", "11px")
			.attr("text-anchor", "middle")
			.text(fz)
			//.style("cursor", "pointer")
			//.on("mouseover", function(d) { d3.select(this).style("font-weight", "bold");})
			//.on("mouseout",  function(d) { d3.select(this).style("font-weight", "normal");})
			//.on("mousedown.drag",  self.xaxis_drag())
			//.on("touchstart.drag", self.xaxis_drag());

		gz.exit().remove();	
		
	self.plot.call(d3.behavior.zoom().x(self.z).y(self.y).on("zoom", self.redraw(dim)));
	self.update(dim)
	
	};
};







