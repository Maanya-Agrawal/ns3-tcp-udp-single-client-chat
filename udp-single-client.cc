/* UDP Single Client-Server Chat in NS3
 * Topology: Client <----> Server (Point-to-Point Link)
 * Save as: scratch/udp-single-client.cc
 * Run with: ./ns3 run scratch/udp-single-client
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

// ── Callbacks to print sent/received messages ─────────────────────────────────

void PacketSentCallback(Ptr<const Packet> packet)
{
    std::cout << "[Client] Sent     " << packet->GetSize() << " bytes"
              << " at time " << Simulator::Now().GetSeconds() << "s\n";
}

void PacketReceivedCallback(Ptr<const Packet> packet, const Address &address)
{
    std::cout << "[Server] Received " << packet->GetSize() << " bytes"
              << " from " << InetSocketAddress::ConvertFrom(address).GetIpv4()
              << " at time " << Simulator::Now().GetSeconds() << "s\n";
}

int main(int argc, char *argv[])
{
    // ── Simulation parameters ────────────────────────────────────────────────
    double      simTime    = 10.0;
    std::string dataRate   = "5Mbps";
    std::string delay      = "2ms";
    uint32_t    packetSize = 1024;      // bytes per packet
    double      interval   = 1.0;       // seconds between packets

    CommandLine cmd;
    cmd.AddValue("simTime",    "Simulation duration in seconds", simTime);
    cmd.AddValue("dataRate",   "Link data rate",                 dataRate);
    cmd.AddValue("delay",      "Link propagation delay",         delay);
    cmd.AddValue("packetSize", "Size of each UDP packet",        packetSize);
    cmd.AddValue("interval",   "Interval between packets (s)",   interval);
    cmd.Parse(argc, argv);

    // ── Create nodes ─────────────────────────────────────────────────────────
    NodeContainer nodes;
    nodes.Create(2);          // nodes.Get(0) = Client, nodes.Get(1) = Server

    // ── Configure Point-to-Point link ────────────────────────────────────────
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue(dataRate));
    p2p.SetChannelAttribute("Delay",    StringValue(delay));

    NetDeviceContainer devices = p2p.Install(nodes);

    // ── Install Internet stack ────────────────────────────────────────────────
    InternetStackHelper internet;
    internet.Install(nodes);

    // ── Assign IP addresses ───────────────────────────────────────────────────
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    Ipv4Address clientAddr = interfaces.GetAddress(0);
    Ipv4Address serverAddr = interfaces.GetAddress(1);

    std::cout << "Client IP : " << clientAddr << "\n";
    std::cout << "Server IP : " << serverAddr << "\n";
    std::cout << "Packet size : " << packetSize << " bytes\n";
    std::cout << "Send interval : " << interval << "s\n";
    std::cout << "──────────────────────────────────────────\n";

    // ── Server: UdpEchoServer ─────────────────────────────────────────────────
    uint16_t port = 9;

    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApp = echoServer.Install(nodes.Get(1));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop (Seconds(simTime));

    // ── Client: UdpEchoClient ─────────────────────────────────────────────────
    uint32_t maxPackets = static_cast<uint32_t>((simTime - 1.0) / interval);

    UdpEchoClientHelper echoClient(serverAddr, port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(maxPackets));
    echoClient.SetAttribute("Interval",   TimeValue(Seconds(interval)));
    echoClient.SetAttribute("PacketSize", UintegerValue(packetSize));

    ApplicationContainer clientApp = echoClient.Install(nodes.Get(0));
    clientApp.Start(Seconds(1.0));
    clientApp.Stop (Seconds(simTime));

    // ── Hook trace callbacks ──────────────────────────────────────────────────
    clientApp.Get(0)->TraceConnectWithoutContext("Tx", MakeCallback(&PacketSentCallback));
    serverApp.Get(0)->TraceConnectWithoutContext("Rx", MakeCallback(&PacketReceivedCallback));

    // ── Enable PCAP tracing (optional) ───────────────────────────────────────
    p2p.EnablePcapAll("udp-single-client");

    // ── Run simulation ────────────────────────────────────────────────────────
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // ── Print summary ─────────────────────────────────────────────────────────
    std::cout << "──────────────────────────────────────────\n";
    std::cout << "Total packets sent     : " << maxPackets << "\n";
    std::cout << "Total bytes sent       : " << maxPackets * packetSize << "\n";
    std::cout << "──────────────────────────────────────────\n";

    Simulator::Destroy();
    return 0;
}

