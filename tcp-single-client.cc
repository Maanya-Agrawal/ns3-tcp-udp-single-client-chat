/* TCP Single Client-Server Communication in NS3
 * Topology: Client <----> Server (Point-to-Point Link)
 * Save as: scratch/tcp-single-client.cc
 * Run with: ./ns3 run scratch/tcp-single-client
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    // ── Simulation parameters ────────────────────────────────────────────────
    uint32_t    maxBytes   = 0;
    double      simTime    = 10.0;
    std::string dataRate   = "5Mbps";
    std::string delay      = "2ms";

    CommandLine cmd;
    cmd.AddValue("maxBytes", "Total bytes to send (0 = unlimited)", maxBytes);
    cmd.AddValue("simTime",  "Simulation duration in seconds",      simTime);
    cmd.AddValue("dataRate", "Link data rate",                      dataRate);
    cmd.AddValue("delay",    "Link propagation delay",              delay);
    cmd.Parse(argc, argv);

    // ── Create nodes ─────────────────────────────────────────────────────────
    NodeContainer nodes;
    nodes.Create(2);

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

    // ── Server: PacketSink ────────────────────────────────────────────────────
    uint16_t port = 9;

    PacketSinkHelper sinkHelper(
        "ns3::TcpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port)
    );

    ApplicationContainer serverApp = sinkHelper.Install(nodes.Get(1));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop (Seconds(simTime));

    // ── Client: BulkSend ──────────────────────────────────────────────────────
    BulkSendHelper bulkSend(
        "ns3::TcpSocketFactory",
        InetSocketAddress(serverAddr, port)
    );

    bulkSend.SetAttribute("MaxBytes", UintegerValue(maxBytes));

    ApplicationContainer clientApp = bulkSend.Install(nodes.Get(0));
    clientApp.Start(Seconds(1.0));
    clientApp.Stop (Seconds(simTime));

    // ── Enable PCAP tracing (optional) ───────────────────────────────────────
    p2p.EnablePcapAll("tcp-single-client");

    // ── Run simulation ────────────────────────────────────────────────────────
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // ── Print results ─────────────────────────────────────────────────────────
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(serverApp.Get(0));
    std::cout << "──────────────────────────────────────────\n";
    std::cout << "Total bytes received by server : " << sink->GetTotalRx() << "\n";
    std::cout << "──────────────────────────────────────────\n";

    Simulator::Destroy();
    return 0;
}

