event bro_init()
{
	print("Loading Classifying Script : Decision Tree");
	#DecisionTree::LoadModel("TreeModel-PingTunnel");
}

#event classify_event(value: feature_vector)
#{
#	#print("Calculating metric");
#	DecisionTree::Classify(value);
#}

event DecisionTree::class_result_event(metric:int)
{
#	if (metric == 1)
#		print("Ping Tunnel detected");
#	else
#		print("Not a ping tunnel");
}
