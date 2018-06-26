#!/usr/bin/env python
import rospy

counter = []
output = False

def callback(msg, index):
    counter[index] += 1
    if output:
        print "---"
        for i in xrange(len(counter)):
            print str(i) + " " + str(counter[i])
        print "---"


if __name__ == "__main__":
    rospy.init_node("message_counter")
    input_topics = rospy.get_param("~input_topics", "")
    output = rospy.get_param("~print_running_results", False)
    run_for_secs = rospy.get_param("~run_for_secs", -1)
    for i in xrange(len(input_topics)):
        rospy.Subscriber(input_topics[i], rospy.AnyMsg, callback, i)
        print "Subscribed to " + input_topics[i]
        counter.append(0)
    if run_for_secs < 0:
        while not rospy.is_shutdown():
            rospy.spin()
    else:
        start_time = rospy.Time.now().to_sec()
        cur_time = start_time
        while (cur_time - start_time < run_for_secs) and not rospy.is_shutdown():
            cur_time = rospy.Time.now().to_sec()
    print "-Final Result-"
    for i in xrange(len(counter)):
        print "Topic name: " + input_topics[i] + ", Messages received: " + str(counter[i])
    print "---"